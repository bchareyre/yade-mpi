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
#include "OpenGLRenderingEngine.hpp"
#include "MessageDialog.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <qfiledialog.h>
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
	setMaximumSize(size());

	parentWorkspace = parent;
	
//	while(! renderer )
// FIXME - what is going on here? it was crashing rabdomly unless I added these lines...
	shared_ptr<Factorable> tmpRenderer = ClassFactory::instance().createShared("OpenGLRenderingEngine");
	shared_ptr<OpenGLRenderingEngine> tmp2 = dynamic_pointer_cast<OpenGLRenderingEngine>(tmpRenderer);
	renderer = dynamic_pointer_cast<RenderingEngine>(tmp2);
	
	if(renderer)
	{
		guiGen.setResizeHeight(true);
		guiGen.setResizeWidth(false);
		guiGen.setShift(10,30);
		guiGen.setShowButtons(false);
		guiGen.buildGUI(renderer, gbRenderingEngineParameters);
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
	terminateAllThreads();
	
	map<int,GLViewer*>::iterator gi = glViews.begin();
	map<int,GLViewer*>::iterator giEnd = glViews.end();
	for(;gi!=giEnd;++gi)
		delete (*gi).second;
	glViews.clear();
	
	Omega::instance().freeSimulation();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SimulationController::terminateAllThreads()
{
	{
		boost::mutex mutex;
		boost::mutex::scoped_lock lock(mutex);
		
		Omega::instance().finishSimulationLoop();
		Omega::instance().joinSimulationLoop();
	}
	
	{
		boost::mutex mutex;
		boost::mutex::scoped_lock lock(mutex);
		
		updater->finish();
		updater->join();
	}
	
	map<int,GLViewer*>::reverse_iterator gi = glViews.rbegin();
	map<int,GLViewer*>::reverse_iterator giEnd = glViews.rend();
	for(;gi!=giEnd;++gi)
	{
		{
			boost::mutex mutex;
			boost::mutex::scoped_lock lock(mutex);
			(*gi).second->finishRendering();
			(*gi).second->joinRendering();
		}
	}
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

	class ScopeTest
	{
		public : ScopeTest() { ok_=false; };
		public : ~ScopeTest()
		{
			if(ok_)
				cerr << "loading file scope left correctly\n";
			else
				cerr << "loading file scope left UNCORRECTLY!\n";
		}
		public : void ok() { ok_=true; };
		private: bool ok_;
	};

	boost::mutex resizeMutex;
	boost::mutex::scoped_lock lock(resizeMutex);

	QString selectedFilter;
	QString fileName = QFileDialog::getOpenFileName("../data", "XML Yade File (*.xml)", 0,"Open File","Choose a file to open",&selectedFilter );
		
	if (!fileName.isEmpty() && selectedFilter == "XML Yade File (*.xml)")
	{
		ScopeTest scopeTest;
			
		map<int,GLViewer*>::iterator gi = glViews.begin();
		map<int,GLViewer*>::iterator giEnd = glViews.end();
		for(;gi!=giEnd;++gi)
			(*gi).second->stopRendering();
			
		updater->stop();
		Omega::instance().finishSimulationLoop();
		Omega::instance().joinSimulationLoop();
	
		Omega::instance().setSimulationFileName(fileName);
		Omega::instance().loadSimulation();
		
		//boost::thread t(&SimulationController::load);
		
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
		
		scopeTest.ok();
	}
} 

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SimulationController::pbNewViewClicked()
{
	boost::mutex resizeMutex;
	boost::mutex::scoped_lock lock(resizeMutex);

	addNewView();
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SimulationController::addNewView()
{
	boost::mutex resizeMutex;
	boost::mutex::scoped_lock lock(resizeMutex);

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
	boost::mutex resizeMutex;
	boost::mutex::scoped_lock lock(resizeMutex);
	Omega::instance().stopSimulationLoop();
	updater->stop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SimulationController::pbStartClicked()
{
	boost::mutex resizeMutex;
	boost::mutex::scoped_lock lock(resizeMutex);
	Omega::instance().startSimulationLoop();	
	updater->start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SimulationController::pbResetClicked()
{
	boost::mutex resizeMutex;
	boost::mutex::scoped_lock lock(resizeMutex);
	updater->stop();
	Omega::instance().finishSimulationLoop();
	Omega::instance().joinSimulationLoop();	
	Omega::instance().loadSimulation();		
	Omega::instance().createSimulationLoop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SimulationController::pbCenterSceneClicked()
{
	boost::mutex resizeMutex;
	boost::mutex::scoped_lock lock(resizeMutex);
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
