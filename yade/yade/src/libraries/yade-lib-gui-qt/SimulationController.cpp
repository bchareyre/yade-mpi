/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "SimulationController.hpp"
#include "Omega.hpp"
#include "ThreadSynchronizer.hpp"
#include "Math.hpp"
#include "Threadable.hpp"
//#include "OpenGLRenderingEngine.hpp"
#include "MessageDialog.hpp"
#include "FileDialog.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace boost;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SimulationController::SimulationController(QWidget * parent) : QtGeneratedSimulationController(parent,"SimulationController")
{
	setMinimumSize(size());
	//setMaximumSize(size());

	parentWorkspace = parent;
	
	scrollViewFrame = new QFrame();	

	scrollView = new QScrollView( gbRenderingEngineParameters, "scrollView" );
	scrollView->setVScrollBarMode(QScrollView::Auto);
	scrollView->setHScrollBarMode(QScrollView::Auto);
	gbRenderingEngineParametersLayout->addWidget( scrollView );
	scrollView->show();	

		
//	while(! renderer )
// FIXME - what is going on here? it was crashing rabdomly unless I added these lines...
	shared_ptr<Factorable> tmpRenderer = ClassFactory::instance().createShared("OpenGLRenderingEngine");
	//shared_ptr<OpenGLRenderingEngine> tmp2 = dynamic_pointer_cast<OpenGLRenderingEngine>(tmpRenderer);
	//renderer = dynamic_pointer_cast<RenderingEngine>(tmp2);
	renderer = static_pointer_cast<RenderingEngine>(tmpRenderer);
	
	if(renderer)
	{
		guiGen.setResizeHeight(true);
		guiGen.setResizeWidth(true);
		guiGen.setShift(10,30);
		guiGen.setShowButtons(false);
		QSize s = scrollView->size();
		scrollViewFrame->resize(s.width(),s.height());
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
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SimulationController::~SimulationController()
{	
	//LOCK(Omega::instance().getRootBodyMutex());
	
	Omega::instance().finishSimulationLoop();
	Omega::instance().joinSimulationLoop();
	
	updater->finish();
	updater->join();
	
	map<int,GLViewer*>::reverse_iterator gi = glViews.rbegin();
	map<int,GLViewer*>::reverse_iterator giEnd = glViews.rend();
	for(;gi!=giEnd;++gi)
	{
		boost::mutex::scoped_lock lock(mutex);
		(*gi).second->finishRendering();
		(*gi).second->joinRendering();
		delete (*gi).second;
	}

	glViews.clear();
	
	Omega::instance().freeRootBody();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SimulationController::pbApplyClicked()
{
	guiGen.deserialize(renderer);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//QWidget* SimulationController::parentWorkspace=0;
// void SimulationController::load()
// {
// 	MessageDialog * d = new MessageDialog("Loading file "+Omega::instance().getSimulationFileName()+". Please wait...", parentWorkspace,"Message Dialog",true);
// 	
// 	parentWorkspace->setEnabled(false);
// 	
// 	d->setEnabled(true);
// 	
/// 	IOManager::loadFromFile("yade-lib-serialization-xml",Omega::instance().getSimulationFileName(),"rootBody",Omega::instance().rootBody);
// 	IOManager::loadFromFile("XMLManager",Omega::instance().getSimulationFileName(),"rootBody",Omega::instance().rootBody);
// 	
// 	parentWorkspace->setEnabled(true);
// 	
// 	delete d;
// 	d = new MessageDialog("File "+Omega::instance().getSimulationFileName()+" was  correctly loaded...", parentWorkspace);
// 	d->exec();
// 	delete d;
// }

void SimulationController::pbLoadClicked()
{
	
	string selectedFilter;
	string fileName = FileDialog::getOpenFileName("../data", "XML Yade File (*.xml)", "Choose a file to open", parentWorkspace, selectedFilter );
		
	if (fileName.size()!=0 && selectedFilter == "XML Yade File (*.xml)" && filesystem::exists(fileName) && filesystem::extension(fileName)==".xml")
	{
		
		map<int,GLViewer*>::iterator gi = glViews.begin();
		map<int,GLViewer*>::iterator giEnd = glViews.end();
		for(;gi!=giEnd;++gi)
			(*gi).second->stopRendering();

		updater->stop();
		Omega::instance().finishSimulationLoop();
		Omega::instance().joinSimulationLoop();
		
		Omega::instance().setSimulationFileName(fileName);
		Omega::instance().loadSimulation();
		
		string fullName = string(filesystem::basename(fileName.data()))+string(filesystem::extension(fileName.data()));
		tlCurrentSimulation->setText(fullName);
		
		Omega::instance().createSimulationLoop();
		
		gi = glViews.begin();
		giEnd = glViews.end();
		for(;gi!=giEnd;++gi)
		{
			(*gi).second->centerScene();
			(*gi).second->startRendering();
		}
		
		pbStartSimulation->setEnabled(true);
		pbStopSimulation->setEnabled(true);
		pbResetSimulation->setEnabled(true);
		pbOneSimulationStep->setEnabled(true);
	}
} 

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SimulationController::pbNewViewClicked()
{
	addNewView();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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
	glViews[maxNbViews]->startRendering();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SimulationController::closeGLViewEvent(int id)
{
	//LOCK(Omega::instance().getRootBodyMutex());
	//boost::mutex::scoped_lock lock(mutex);

	if (id!=0)
	{
		glViews[id]->finishRendering();
		glViews[id]->joinRendering();
		delete glViews[id];
		glViews.erase(id);
		if (id==maxNbViews)
			maxNbViews--;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SimulationController::pbStopClicked()
{
	//LOCK(Omega::instance().getRootBodyMutex());
	Omega::instance().stopSimulationLoop();
	updater->stop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SimulationController::pbStartClicked()
{
	//LOCK(Omega::instance().getRootBodyMutex());
	Omega::instance().startSimulationLoop();	
	updater->start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SimulationController::pbResetClicked()
{	
	updater->stop();
	map<int,GLViewer*>::iterator gi = glViews.begin();
	map<int,GLViewer*>::iterator giEnd = glViews.end();
	for(;gi!=giEnd;++gi)
		(*gi).second->stopRendering();
	
	Omega::instance().finishSimulationLoop();
	Omega::instance().joinSimulationLoop();	
	Omega::instance().loadSimulation();
	Omega::instance().createSimulationLoop();

	updater->oneLoop(); // to refresh gui
	
	gi = glViews.begin();
	for(;gi!=giEnd;++gi)
		(*gi).second->startRendering();

	pbCenterSceneClicked();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SimulationController::pbOneSimulationStepClicked()
{
	//Omega::instance().stopSimulationLoop();
	//updater->start();
	//FIXME : fix real simulation time
	Omega::instance().doOneSimulationLoop();
	updater->doOneLoop();
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SimulationController::pbCenterSceneClicked()
{
	//LOCK(Omega::instance().getRootBodyMutex());
	map<int,GLViewer*>::iterator gi = glViews.begin();
	map<int,GLViewer*>::iterator giEnd = glViews.end();
	for(;gi!=giEnd;++gi)
		(*gi).second->centerScene();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SimulationController::closeEvent(QCloseEvent *)
{
	emit closeSignal();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
