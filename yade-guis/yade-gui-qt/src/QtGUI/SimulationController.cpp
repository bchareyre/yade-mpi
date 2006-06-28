/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "SimulationController.hpp"
#include "MessageDialog.hpp"
#include "FileDialog.hpp"
#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qradiobutton.h>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <unistd.h>
#include <yade/yade-core/Omega.hpp>
#include <yade/yade-lib-threads/ThreadSynchronizer.hpp>
#include <yade/yade-lib-wm3-math/Math.hpp>
#include <yade/yade-lib-threads/Threadable.hpp>


using namespace boost;


SimulationController::SimulationController(QWidget * parent) : QtGeneratedSimulationController(parent,"SimulationController")
{
	parentWorkspace = parent;

	scrollViewFrame = new QFrame();	
	
	scrollViewLayout = new QVBoxLayout( scrollViewOutsideFrame, 0, 0, "scrollViewLayout"); 
	
	scrollView = new QScrollView( scrollViewOutsideFrame, "scrollView" );
	scrollView->setVScrollBarMode(QScrollView::Auto);
	scrollView->setHScrollBarMode(QScrollView::Auto);
	scrollViewLayout->addWidget( scrollView );
	scrollView->show();	

	shared_ptr<Factorable> tmpRenderer = ClassFactory::instance().createShared("OpenGLRenderingEngine");
	renderer = static_pointer_cast<RenderingEngine>(tmpRenderer);
	
	if(renderer)
	{
		guiGen.setResizeHeight(true);
		guiGen.setResizeWidth(true);
		guiGen.setShift(10,30);
		guiGen.setShowButtons(false);
		QSize s = scrollView->size();
		scrollViewFrame->resize(s.width(),s.height());
		
		filesystem::path rendererConfig = filesystem::path( Omega::instance().yadeConfigPath + "/OpenGLRendererPref.xml", filesystem::native);
		if ( filesystem::exists( rendererConfig ) )
		{
			try
			{
				cerr << "loading configuration file: " << rendererConfig.string() << "\n";
				IOFormatManager::loadFromFile("XMLFormatManager",rendererConfig.string(),"renderer",renderer);
			}
			catch(SerializableError& e)
			{
				shared_ptr<MessageDialog> md = shared_ptr<MessageDialog>(new MessageDialog(rendererConfig.string() + " exists, but cannot be loaded: " + e.what(),this->parentWidget()->parentWidget()));
				md->exec(); 
			}
		}
		
		guiGen.buildGUI(renderer, scrollViewFrame);
		scrollView->addChild(scrollViewFrame);
	}
	else
	{
		cerr << "renderer not created - why?!\n";
	}
	
	maxNbViews=0;
	addNewView();

//	updater = shared_ptr<SimulationControllerUpdater>(new SimulationControllerUpdater(this));
}


SimulationController::~SimulationController()
{	
	//LOCK(Omega::instance().getRootBodyMutex());
	
	Omega::instance().finishSimulationLoop();
	Omega::instance().joinSimulationLoop();
	
//	updater->finish();
//	updater->join();
	
	map<int,GLViewer*>::reverse_iterator gi = glViews.rbegin();
	map<int,GLViewer*>::reverse_iterator giEnd = glViews.rend();
	for(;gi!=giEnd;++gi)
	{
		//boost::mutex::scoped_lock lock(mutex);
		//(*gi).second->finishRendering();
		//(*gi).second->joinRendering();
		//delete (*gi).second;
		gi->second->close();
		delete gi->second;
	}

	glViews.clear();
	
	Omega::instance().freeRootBody();
	
	filesystem::path rendererConfig = filesystem::path( Omega::instance().yadeConfigPath + "/OpenGLRendererPref.xml", filesystem::native);
	IOFormatManager::saveToFile("XMLFormatManager",rendererConfig.string(),"renderer",renderer);
}

void SimulationController::redrawAll()
{
	map<int,GLViewer*>::reverse_iterator gi = glViews.rbegin();
	map<int,GLViewer*>::reverse_iterator giEnd = glViews.rend();
	for(;gi!=giEnd;++gi)
		gi->second->updateGL();
}

void SimulationController::pbApplyClicked()
{
	guiGen.deserialize(renderer);
}


void SimulationController::pbLoadClicked()
{
	string selectedFilter;
	std::vector<string> filters;
	filters.push_back("Yade Binary File (*.yade)");
	filters.push_back("XML Yade File (*.xml)");
	string fileName = FileDialog::getOpenFileName("../data", filters, "Choose a file to open", parentWorkspace, selectedFilter );
		
	if ( 	   fileName.size()!=0 
		&& (selectedFilter == "XML Yade File (*.xml)" || selectedFilter == "Yade Binary File (*.yade)") 
		&& filesystem::exists(fileName) 
		&& (filesystem::extension(fileName)==".xml" || filesystem::extension(fileName)==".yade"))
	{
		
		map<int,GLViewer*>::iterator gi = glViews.begin();
		map<int,GLViewer*>::iterator giEnd = glViews.end();
	//	for(;gi!=giEnd;++gi)
	//		(*gi).second->stopRendering();

//		updater->stop();
		Omega::instance().finishSimulationLoop();
		Omega::instance().joinSimulationLoop();
		
		Omega::instance().setSimulationFileName(fileName);
		try
		{
			Omega::instance().loadSimulation(); // expecting throw here.
			string fullName = string(filesystem::basename(fileName.data()))+string(filesystem::extension(fileName.data()));
			tlCurrentSimulation->setText(fullName); 
			Omega::instance().createSimulationLoop();
			
			pbStartSimulation->setEnabled(true);
			pbStopSimulation->setEnabled(true);
			pbResetSimulation->setEnabled(true);
			pbOneSimulationStep->setEnabled(true);

			changeSkipTimeStepper = true;
			if (Omega::instance().containTimeStepper())
			{
				rbTimeStepper->setEnabled(true);
				rbTimeStepper->setChecked(true);
				wasUsingTimeStepper = true;
			}
			else
			{
				rbTimeStepper->setEnabled(false);
				rbFixed->setChecked(true);
				wasUsingTimeStepper = false;
			}
		} 
		catch(SerializableError& e) // catching it...
		{
			Omega::instance().freeRootBody();
			shared_ptr<MessageDialog> md = shared_ptr<MessageDialog>(new MessageDialog(e.what(),this->parentWidget()->parentWidget()));
			md->exec(); 
			pbStartSimulation->setDisabled(true);
			pbStopSimulation->setDisabled(true);
			pbResetSimulation->setDisabled(true);
			pbOneSimulationStep->setDisabled(true);
		}
		
		gi = glViews.begin();
		giEnd = glViews.end();
		for(;gi!=giEnd;++gi)
		{
			(*gi).second->centerScene();
	//		(*gi).second->startRendering();
		}
		redrawAll();
	}
} 

void SimulationController::pbSaveClicked()
{
	pbStopClicked();

	string selectedFilter;
	std::vector<string> filters;
	filters.push_back("Yade Binary File (*.yade)");
	filters.push_back("XML Yade File (*.xml)");
	string fileName = FileDialog::getSaveFileName("../data", filters, "Specify file name to save", parentWorkspace, selectedFilter );

	if ( 	   fileName.size()!=0 
		&& (selectedFilter == "XML Yade File (*.xml)" || selectedFilter == "Yade Binary File (*.yade)" ) 
		&& (filesystem::extension(fileName)==".xml" || filesystem::extension(fileName)==".yade" || filesystem::extension(fileName)=="" )
		&& (fileName != "")
		&& (fileName != "/")
		&& (fileName != "../data"))
	{
	//	map<int,GLViewer*>::iterator gi = glViews.begin();
	//	map<int,GLViewer*>::iterator giEnd = glViews.end();
	//	for(;gi!=giEnd;++gi)
	//		(*gi).second->stopRendering();

		if(filesystem::extension(fileName)=="") // user forgot to specify extension - fix it.
			fileName += (selectedFilter == "XML Yade File (*.xml)") ? ".xml" : ".yade";
		
		cerr << "saving simulation: " << fileName << "\n";
		Omega::instance().saveSimulation(fileName);
		
	//	gi = glViews.begin();
	//	giEnd = glViews.end();
	//	for(;gi!=giEnd;++gi)
	//		(*gi).second->startRendering();
		redrawAll();
	}
	else
	{
		shared_ptr<MessageDialog> md = shared_ptr<MessageDialog>(new MessageDialog("Save failed - bad file extension.",this->parentWidget()->parentWidget()));
		md->exec(); 
	}
}

void SimulationController::pbNewViewClicked()
{
	addNewView();
}


void SimulationController::addNewView()
{
	//LOCK(Omega::instance().getRootBodyMutex());
	
	QGLFormat format;
	QGLFormat::setDefaultFormat( format );
	format.setStencil(TRUE);
	format.setAlpha(TRUE);

	if (glViews.size()==0)
	{
		glViews[0] = new GLViewer(0,renderer,format,parentWorkspace);
		maxNbViews = 0;
	}
	else
	{
		maxNbViews++;
		glViews[maxNbViews] = new GLViewer(maxNbViews,renderer, format, parentWorkspace, glViews[0]);
	}
	
	connect( glViews[maxNbViews], SIGNAL( closeSignal(int) ), this, SLOT( closeGLViewEvent(int) ) );
	glViews[maxNbViews]->centerScene();
	//glViews[maxNbViews]->startRendering();
}


void SimulationController::closeGLViewEvent(int id)
{
	//LOCK(Omega::instance().getRootBodyMutex());
	//boost::mutex::scoped_lock lock(mutex);

	if (id!=0)
	{
	//	glViews[id]->finishRendering();
	//	glViews[id]->joinRendering();
		glViews[id]->close();
		delete glViews[id];
		glViews.erase(id);
		if (id==maxNbViews)
			maxNbViews--;
	}
}


void SimulationController::pbStopClicked()
{
	//LOCK(Omega::instance().getRootBodyMutex());
	Omega::instance().stopSimulationLoop();
	map<int,GLViewer*>::reverse_iterator gi = glViews.rbegin();
	map<int,GLViewer*>::reverse_iterator giEnd = glViews.rend();
	for(;gi!=giEnd;++gi)
		gi->second->stopAnimation();
//	updater->stop();
}


void SimulationController::pbStartClicked()
{
	//LOCK(Omega::instance().getRootBodyMutex());
	Omega::instance().startSimulationLoop();	
	map<int,GLViewer*>::reverse_iterator gi = glViews.rbegin();
	map<int,GLViewer*>::reverse_iterator giEnd = glViews.rend();
	for(;gi!=giEnd;++gi)
		gi->second->startAnimation();
//	updater->start();
}


void SimulationController::pbResetClicked()
{	
//	updater->stop();
	//map<int,GLViewer*>::iterator gi = glViews.begin();
	//map<int,GLViewer*>::iterator giEnd = glViews.end();
	//for(;gi!=giEnd;++gi)
	//	(*gi).second->stopRendering();
	
	Omega::instance().finishSimulationLoop();
	Omega::instance().joinSimulationLoop();	
	Omega::instance().loadSimulation();
	Omega::instance().createSimulationLoop();

//	updater->oneLoop(); // to refresh gui
	
	//gi = glViews.begin();
	//for(;gi!=giEnd;++gi)
	//	(*gi).second->startRendering();

	changeSkipTimeStepper = true;
	skipTimeStepper = !wasUsingTimeStepper;

	rbTimeStepper->setEnabled(Omega::instance().containTimeStepper());
	redrawAll();

	//pbCenterSceneClicked();
}


void SimulationController::pbOneSimulationStepClicked()
{
	//Omega::instance().stopSimulationLoop();
	//updater->start();
	//FIXME : fix real simulation time
	pbStopClicked();
	Omega::instance().doOneSimulationLoop();
	redrawAll();
//	updater->doOneLoop();
	
}


void SimulationController::pbCenterSceneClicked()
{
	//LOCK(Omega::instance().getRootBodyMutex());
	map<int,GLViewer*>::iterator gi = glViews.begin();
	map<int,GLViewer*>::iterator giEnd = glViews.end();
	for(;gi!=giEnd;++gi)
		(*gi).second->centerScene();
//	redrawAll();
}


void SimulationController::closeEvent(QCloseEvent *)
{
	pbStopClicked();
	emit closeSignal();
}


void SimulationController::bgTimeStepClicked(int i)
{
	switch (i)
	{
		case 0 : //Use timeStepper
			changeSkipTimeStepper = true;
			skipTimeStepper = false;
			wasUsingTimeStepper=true;
			break;
		case 1 : // Try RealTime
			changeSkipTimeStepper = true;
			skipTimeStepper = true;
			wasUsingTimeStepper=false;
			break;
		case 2 : // use fixed time Step
			changeSkipTimeStepper = true;
			skipTimeStepper = true;
			changeTimeStep = true;
			wasUsingTimeStepper=false;
			break;
		default: break;
	}

}


void SimulationController::sb10PowerSecondValueChanged(int)
{
	changeTimeStep = true;
}


void SimulationController::sbSecondValueChanged(int)
{
	changeTimeStep = true;
}

