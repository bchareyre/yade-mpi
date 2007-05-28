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
#include<yade/core/Omega.hpp>
#include<yade/core/yadeExceptions.hpp>
#include <Wm3Math.h>
#include<yade/lib-base/yadeWm3.hpp>


using namespace boost;


SimulationController::SimulationController(QWidget * parent) : QtGeneratedSimulationController(parent,"SimulationController")
{
	sync=false;
	refreshTime = 20;
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

	updater = shared_ptr<SimulationControllerUpdater>(new SimulationControllerUpdater(this));

	// HACK: this should be passed through the bazillion of abstract interfaces to the the most abstract idea,
	// of which Plato speaks in the Republic and elsewhere, then descending from this perfectly one beyond being
	// though the cascades of emanations here, where it incarnates. Unfortunately, there is no heaven in c++, because
	// c++ itself is hell.
	//
	// Actually, it is quite useful, since it allows me to pass simulation to yade from command line,
	// thus saving at least 4 mouse-clicks
	//
	// there is file existence assertion in lodSimulationFromFilename, so yade will abort cleanly...
	cerr<<"Omega::instance().getSimulationFileName()="<<Omega::instance().getSimulationFileName()<<endl;
	if (Omega::instance().getSimulationFileName()!=""){
		loadSimulationFromFileName(Omega::instance().getSimulationFileName());
	}

	// this should prevent weird things happening during the very first run; it is not a proper fix, though.
	pbStopClicked();
}


SimulationController::~SimulationController()
{
	Omega::instance().finishSimulationLoop();
	Omega::instance().joinSimulationLoop();

	// to avoid loading that file next time, when SimulationController is opened again.
	Omega::instance().setSimulationFileName("");

	map<int,GLViewer*>::reverse_iterator gi = glViews.rbegin();
	map<int,GLViewer*>::reverse_iterator giEnd = glViews.rend();
	for(;gi!=giEnd;++gi)
	{
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
	redrawAll();
}


void SimulationController::pbLoadClicked()
{
	pbStopClicked();

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
		this->loadSimulationFromFileName(fileName);
	}
}


void SimulationController::loadSimulationFromFileName(const std::string& fileName,bool center)
{
	assert(filesystem::exists(fileName));

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
			Omega::instance().setSimulationFileName("");
			shared_ptr<MessageDialog> md = shared_ptr<MessageDialog>(new MessageDialog(e.what(),this->parentWidget()->parentWidget()));
			md->exec(); 
			pbStartSimulation->setDisabled(true);
			pbStopSimulation->setDisabled(true);
			pbResetSimulation->setDisabled(true);
			pbOneSimulationStep->setDisabled(true);
		}
		catch(yadeError& e)
		{
			Omega::instance().freeRootBody();
			Omega::instance().setSimulationFileName("");
			shared_ptr<MessageDialog> md = shared_ptr<MessageDialog>(new MessageDialog(e.what(),this->parentWidget()->parentWidget()));
			md->exec(); 
			pbStartSimulation->setDisabled(true);
			pbStopSimulation->setDisabled(true);
			pbResetSimulation->setDisabled(true);
			pbOneSimulationStep->setDisabled(true);
		}

		if(center)
			pbCenterSceneClicked();
		
		rbTimeStepper->setEnabled(Omega::instance().containTimeStepper());
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

		if(filesystem::extension(fileName)=="") // user forgot to specify extension - fix it.
			fileName += (selectedFilter == "XML Yade File (*.xml)") ? ".xml" : ".yade";

		cerr << "saving simulation: " << fileName << "\n";
		Omega::instance().saveSimulation(fileName);

		redrawAll();
	}
	else
	{
		shared_ptr<MessageDialog> md = shared_ptr<MessageDialog>(new MessageDialog("Save aborted (check file extension).",this->parentWidget()->parentWidget()));
		md->exec(); 
	}
}

void SimulationController::pbNewViewClicked()
{
	addNewView();
}


void SimulationController::addNewView()
{
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
}


void SimulationController::closeGLViewEvent(int id)
{
	if (id!=0)
	{
		glViews[id]->close();
		delete glViews[id];
		glViews.erase(id);
		if (id==maxNbViews)
			maxNbViews--;
	}
}


void SimulationController::pbStopClicked()
{
	Omega::instance().stopSimulationLoop();
	killTimers();
}


void SimulationController::pbStartClicked()
{
	if(!sync)
		Omega::instance().startSimulationLoop();        

	startTimer(refreshTime);
}


void SimulationController::pbResetClicked()
{
//	updater->stop();

	pbStopClicked();

/*
	Omega::instance().finishSimulationLoop();
	Omega::instance().joinSimulationLoop(); 
	Omega::instance().loadSimulation();
	Omega::instance().createSimulationLoop();
*/
	std::string name=Omega::instance().getSimulationFileName(); 
	loadSimulationFromFileName(name,false);

	if(Omega::instance().getRootBody())
	{
		changeSkipTimeStepper = true;
		skipTimeStepper = !wasUsingTimeStepper;
		updater->oneLoop(); // to refresh gui

		rbTimeStepper->setEnabled(Omega::instance().containTimeStepper());
		redrawAll();
	} 
	else
	{
		Omega::instance().setSimulationFileName(name);
		pbResetSimulation->setEnabled(true);
	}

//	pbCenterSceneClicked();  // FIXME - add autocenter option...
}


void SimulationController::pbOneSimulationStepClicked()
{
	//updater->start();
	//FIXME : fix real simulation time
	pbStopClicked();
	Omega::instance().spawnSingleSimulationLoop();
	redrawAll();
	updater->oneLoop();
}


void SimulationController::pbCenterSceneClicked()
{
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
	switch (i) // FIXME wtf ? (code written by Olivier)
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

void SimulationController::sbRefreshValueChanged(int v)
{
	pbStopClicked();
	refreshTime = v;
	if(sync && refreshTime < 20) 
		// FIXME - problem is that ThreadRunner cannot 'too often' call spawnSingleSimulationLoop().
		// This is a temporary solution...
		refreshTime = 20;
}

void SimulationController::cbSyncToggled( bool b)
{
	pbStopClicked();
	sync = b;
	if(sync && refreshTime < 20)
		refreshTime = 20;
}

void SimulationController::pbStart2Clicked()
{
	pbStartClicked();
}

void SimulationController::timerEvent( QTimerEvent* )
{
	updater->oneLoop();
	redrawAll();
	if(sync)
		Omega::instance().spawnSingleSimulationLoop();
}

