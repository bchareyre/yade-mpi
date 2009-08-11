/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include "QtGUI.hpp"
#include "YadeQtMainWindow.hpp"
#include "SimulationController.hpp"
#include "MessageDialog.hpp"
#include "FileDialog.hpp"
#include "YadeCamera.hpp"
#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <unistd.h>
#include <Wm3Math.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<boost/version.hpp>
#include<boost/foreach.hpp>
#	ifndef FOREACH
#		define FOREACH BOOST_FOREACH
#	endif

#ifdef YADE_PYTHON
	#include<boost/python.hpp>
#endif


CREATE_LOGGER(SimulationController);

using namespace boost;
/* in sync mode:
 * 	SimulationController::timerEvent handles both redraw (by calling YadeQtMainWindow::redrawAll(force=true) and spawning one simulation loop
 *
 * in async mode:
 * 	YadeQtMainWindow::timerEvent is responsible for doing redraw (will not be done if in sync mode)
 */


SimulationController::SimulationController(QWidget * parent) : QtGeneratedSimulationController(parent,"SimulationController"), iterPerSec_TTL_ms(1000)
{
	sync=false; syncRunning=false;
	refreshTime = 40;
	changeTimeStep=false;
	lastRenderedIteration=-1;

	iterPerSec_LastIter=Omega::instance().getCurrentIteration();
	iterPerSec_LastLocalTime=microsec_clock::local_time();

	parentWorkspace = parent;

	scrollViewFrame = new QFrame();	
	
	scrollViewLayout = new QVBoxLayout( scrollViewOutsideFrame, 0, 0, "scrollViewLayout"); 
	
	scrollView = new QScrollView( scrollViewOutsideFrame, "scrollView" );
	scrollView->setVScrollBarMode(QScrollView::Auto);
	scrollView->setHScrollBarMode(QScrollView::Auto);
	scrollViewLayout->addWidget(scrollView);
	scrollView->show();

	YadeQtMainWindow::self->ensureRenderer(); /* create renderer if none, load its config */

	guiGen.setResizeHeight(true);
	guiGen.setResizeWidth(true);
	guiGen.setShift(10,30);
	guiGen.setShowButtons(false);
	QSize s = scrollView->size();
	scrollViewFrame->resize(s.width(),s.height());
	guiGen.buildGUI(YadeQtMainWindow::self->renderer,scrollViewFrame);
	scrollView->addChild(scrollViewFrame);
	
	maxNbViews=0;
	//addNewView(); // postpone until a file is loaded

	// there is file existence assertion in loadSimulationFromFilename, so yade will abort cleanly...
	LOG_DEBUG("Omega's simulation filename: `"<<Omega::instance().getSimulationFileName()<<"'");
	if (Omega::instance().getSimulationFileName()!="" && (!Omega::instance().getRootBody() || (Omega::instance().getRootBody()->bodies->size()==0 && Omega::instance().getRootBody()->engines.size()==0))){
		loadSimulationFromFileName(Omega::instance().getSimulationFileName());
	}
	else{ LOG_DEBUG("Not loading simulation in ctor"); }

	int mantissa, exponent;
	dtIntegerMantissaExponent(mantissa,exponent);
	sb10PowerSecond->setValue(exponent);
	sbSecond->setValue(mantissa);

	// run timer ANY TIME (simulation may be started asynchronously)
	updateTimerId=startTimer(refreshTime);

	#ifndef YADE_PYTHON
		pyOneliner->setEnabled(false);
		pyOneliner->setText("Yade compiled without Python");
	#endif
}

/* restart timer with SimulationController::refreshTime */
void SimulationController::restartTimer()
{ 
	killTimer(updateTimerId); 
	updateTimerId=startTimer(refreshTime); 
}


void SimulationController::pbApplyClicked()
{
	guiGen.deserialize(YadeQtMainWindow::self->renderer);
	YadeQtMainWindow::self->redrawAll(true);
}
		
void SimulationController::pbXYZ_clicked()
{
	YadeQtMainWindow::self->adjustCameraInCurrentView(qglviewer::Vec(0,1,0),qglviewer::Vec(0,0,-1));
};

void SimulationController::pbYZX_clicked()
{
	YadeQtMainWindow::self->adjustCameraInCurrentView(qglviewer::Vec(0,0,1),qglviewer::Vec(-1,0,0));
};

void SimulationController::pbZXY_clicked()
{
	YadeQtMainWindow::self->adjustCameraInCurrentView(qglviewer::Vec(1,0,0),qglviewer::Vec(0,-1,0));
};


/* enter was pressed in the line-entry;
   execute the command and make the line empty
*/
void SimulationController::pyOnelinerEnter(){
#ifdef YADE_PYTHON
	PyGILState_STATE gstate;
		gstate = PyGILState_Ensure();
		try{
			python::object main=python::import("__main__");
			python::object global=main.attr("__dict__");
			python::exec(pyOneliner->text().ascii(),global,global);
		} catch (const python::error_already_set& e){
			LOG_ERROR("Error from python...");
			PyErr_Print();
		}
	PyGILState_Release(gstate);
	pyOneliner->clear();
#endif
};

void SimulationController::pbLoadClicked()
{
	pbStopClicked();

	string selectedFilter;
	std::vector<string> filters;
	filters.push_back("Yade Binary File (*.yade)");
	filters.push_back("XML Yade File (*.xml *.xml.gz *.xml.bz2)");
	string fileName = FileDialog::getOpenFileName(".", filters, "Choose a file to open", parentWorkspace, selectedFilter );
		
	if (fileName.size()!=0)
	{
		this->loadSimulationFromFileName(fileName);
	}
}


void SimulationController::loadSimulationFromFileName(const std::string& fileName,bool center)
{
	assert(filesystem::exists(fileName) || fileName.find(":memory:")==(size_t)0);

		deactivateControlsWhenLoading();

		Omega::instance().finishSimulationLoop();
		Omega::instance().joinSimulationLoop();

		bool keepTimeStepperSettings=Omega::instance().getSimulationFileName()==fileName;
		Real prevDt=Omega::instance().getTimeStep();
		bool timeStepperUsed=Omega::instance().timeStepperActive();

		Omega::instance().setSimulationFileName(fileName);
		try
		{
			//boost::mutex::scoped_lock lock(timeMutex);

			Omega::instance().loadSimulation(); // expecting throw here.
			string fullName = string(filesystem::basename(fileName.data()))+string(filesystem::extension(fileName.data()));
			//tlCurrentSimulation->setText(fullName); 
			Omega::instance().createSimulationLoop();
			
			pbStartSimulation->setEnabled(true);
			pbStopSimulation->setEnabled(true);
			pbResetSimulation->setEnabled(true);
			pbOneSimulationStep->setEnabled(true);

			if(keepTimeStepperSettings){
				LOG_DEBUG("The same simulation loaded again, keeping time stepper settings.");
				// if we were using fixed time, set that fixed value now
				if(!timeStepperUsed) { Omega::instance().setTimeStep(prevDt); LOG_DEBUG("Using previous fixed time step "<<prevDt);}
				// recover whether timestepper was used or not
				Omega::instance().skipTimeStepper(!timeStepperUsed);
				LOG_DEBUG("Timestepper is "<<(timeStepperUsed?"":"NOT ")<<"being used.");
			}
			else {
				LOG_DEBUG("New simulation loaded, timestepper is "<<(Omega::instance().timeStepperActive()?"":"NOT ")<<"being used (as per XML).");
			}
		}
		 
		catch(SerializableError& e) // catching it...
		{
			Omega::instance().resetRootBody();
			Omega::instance().setSimulationFileName("");
			LOG_ERROR(e.what());
			shared_ptr<MessageDialog> md = shared_ptr<MessageDialog>(new MessageDialog(e.what(),NULL /*this->parentWidget()->parentWidget()*/));
			md->exec(); 
			pbStartSimulation->setDisabled(true);
			pbStopSimulation->setDisabled(true);
			pbResetSimulation->setDisabled(true);
			pbOneSimulationStep->setDisabled(true);
		}
		catch(std::runtime_error& e)
		{
			Omega::instance().resetRootBody();
			Omega::instance().setSimulationFileName("");
			LOG_ERROR(e.what());
			shared_ptr<MessageDialog> md = shared_ptr<MessageDialog>(new MessageDialog(e.what(),NULL /*this->parentWidget()->parentWidget()*/));
			md->exec(); 
			pbStartSimulation->setDisabled(true);
			pbStopSimulation->setDisabled(true);
			pbResetSimulation->setDisabled(true);
			pbOneSimulationStep->setDisabled(true);
		}

		if(center) pbCenterSceneClicked();
		if(YadeQtMainWindow::self->viewsSize() == 0) addNewView();
}

void SimulationController::pbSaveClicked()
{
	pbStopClicked();

	string selectedFilter;
	std::vector<string> filters;
	filters.push_back("Yade Binary File (*.yade)");
	filters.push_back("XML Yade File (*.xml *.xml.gz *.xml.bz2)");
	string fileName = FileDialog::getSaveFileName(".", filters, "Specify file name to save", parentWorkspace, selectedFilter );

	if(fileName.size()!=0  && (fileName != "/")&& (fileName != "."))
	{

		if(filesystem::extension(fileName)=="") // user forgot to specify extension - fix it.
			fileName+=".xml";

		cerr << "saving simulation: " << fileName << "\n";
		Omega::instance().saveSimulation(fileName);

	}
	else
	{
		shared_ptr<MessageDialog> md = shared_ptr<MessageDialog>(new MessageDialog("Save aborted (check file extension).",NULL));
		md->exec(); 
	}
}

void SimulationController::pbNewViewClicked() {	addNewView(); }
void SimulationController::addNewView(){ YadeQtMainWindow::self->createView(); }
void SimulationController::pbCenterSceneClicked() { YadeQtMainWindow::self->centerViews(); }
void SimulationController::closeEvent(QCloseEvent *){ /* switch to async run if running */ if(syncRunning) cbSyncToggled(false); emit closeSignal(); }
void SimulationController::pbStopClicked() { Omega::instance().stopSimulationLoop(); syncRunning=false; }
void SimulationController::pbOneSimulationStepClicked(){pbStopClicked();Omega::instance().spawnSingleSimulationLoop();}
void SimulationController::pbReferenceClicked() {if(YadeQtMainWindow::self->renderer) YadeQtMainWindow::self->renderer->setBodiesRefSe3(Omega::instance().getRootBody());}
void SimulationController::pbStart2Clicked() { pbStartClicked(); }
void SimulationController::pbStartClicked(){
	restartTimer();
	if(sync) syncRunning=true;
	else Omega::instance().startSimulationLoop();
}

void SimulationController::cbSyncToggled(bool b)
{	
	sync=b; 
	if(sync && refreshTime<20) 
		refreshTime=20; 
	pbStopClicked(); 
	pbStartClicked(); 
}

void SimulationController::timerEvent( QTimerEvent* )
{
	doUpdate(); /* update the controller, like iteration number etc */
	if(hasSimulation) // && (Omega::instance().isRunning() || syncRunning || lastRenderedIteration!=Omega::instance().getCurrentIteration()))
	{
		/* update GLViews */
		YadeQtMainWindow::self->redrawAll(true);
		lastRenderedIteration=Omega::instance().getCurrentIteration();
		if(sync && syncRunning){ Omega::instance().spawnSingleSimulationLoop(); }
	}
}

void SimulationController::pbResetClicked()
{
	pbStopClicked();

	std::string name=Omega::instance().getSimulationFileName(); 
	loadSimulationFromFileName(name,false /* don't re-center scene */);

	if(Omega::instance().getRootBody())
	{
		// timeStepper setup done in loadSimulationFromFileName
	} 
	else
	{
		Omega::instance().setSimulationFileName(name);
		pbResetSimulation->setEnabled(true);
	}
	YadeQtMainWindow::self->redrawAll(true);
//	pbCenterSceneClicked();  // FIXME - add autocenter option...
}






void SimulationController::bgTimeStepClicked(int i)
{
	// cerr<<"CHANGING TIME STEP: i="<<i<<endl;
	/* i: buttonGroupId, which is 0 for timeStepper, 2 for fixed step */
	switch (i)
	{
		case 0 : {//Use timeStepper
			wasUsingTimeStepper=true;
			Omega::instance().skipTimeStepper(false);
			break;
			}
		case 1 : // Try RealTime -- deprecated
			throw logic_error("RealTime timestep is deprecated and you couldn't click on it!");
			break;
		case 2 : {// use fixed time Step
			changeTimeStep = true;
			wasUsingTimeStepper=false;
			Omega::instance().skipTimeStepper(true);
			if(sbSecond->value()==0){ sbSecond->setValue(9); sb10PowerSecond->setValue(sb10PowerSecond->value()-1); }
			if(sbSecond->value()==10){ sbSecond->setValue(1); sb10PowerSecond->setValue(sb10PowerSecond->value()+1); }
			int second=(sbSecond->value()), powerSecond = (sb10PowerSecond->value());
			int exp10,mantissa; dtIntegerMantissaExponent(mantissa,exp10);
			// only change timestep if the current timestep would have different representation in this integral thing
			// important so that merely opening the controller doesn't round the existing timestep
			if((mantissa!=second) || (exp10!=powerSecond)){
				LOG_DEBUG("Change timestep: current "<<mantissa<<"^"<<exp10<<"; gui "<<second<<"^"<<powerSecond);
				Omega::instance().setTimeStep((Real)second*Mathr::Pow(10.,(Real)powerSecond));
			}
			break;
		}
		default: break;
	}

}


void SimulationController::sb10PowerSecondValueChanged(int)
{
//	if(!rbFixed->isOn() && userChangingTimestep){ rbFixed->toggle(); bgTimeStepClicked(2); } // this should do the callback as if user clicked fixed timestepper button
//	changeTimeStep = true;
	//assert(rbFixed->isOn()); 
	if(rbFixed->isOn()) bgTimeStepClicked(2);
}


void SimulationController::sbSecondValueChanged(int)
{ 
//	if(!rbFixed->isOn() && userChangingTimestep){ rbFixed->toggle(); bgTimeStepClicked(2); } // dtto
//	changeTimeStep = true;
	//assert(rbFixed->isOn());
	if(rbFixed->isOn()) bgTimeStepClicked(2);
}

void SimulationController::sbRefreshValueChanged(int v)
{
	//pbStopClicked();
	refreshTime = v;
	if(sync && refreshTime < 20) 
		// FIXME - problem is that ThreadRunner cannot 'too often' call spawnSingleSimulationLoop().
		// This is a temporary solution...
		refreshTime = 20;
	restartTimer();
}

void SimulationController::deactivateControlsWhenLoading(){
	labelSimulTime->setText(string(""));
	labelRealTime->setText(string(""));
	labelIter->setText(string(""));
	labelStep->setText(string(""));
	labelEstimationTime->setText(string(""));
	labelStopAtIter->setText(string(""));
	tlCurrentSimulation->setText("[loading]");
	pbStartSimulation->setEnabled(false);
	pbStopSimulation->setEnabled(false);
	pbResetSimulation->setEnabled(false);
	pbOneSimulationStep->setEnabled(false);
	rbFixed->setEnabled(false);
	rbTimeStepper->setEnabled(false);
	sbSecond->setEnabled(false);
	sb10PowerSecond->setEnabled(false);
}


void SimulationController::doUpdate(){
	#if BOOST_VERSION<103500
		boost::try_mutex::scoped_try_lock lock(Omega::instance().renderMutex);
		if(!lock.locked()){
	#else
		boost::try_mutex::scoped_try_lock lock(Omega::instance().renderMutex,boost::try_to_lock);
		if(!lock.owns_lock()){
	#endif
		deactivateControlsWhenLoading();
		return;
	}
	// if we got the lock, update controls as normally

	Real simulationTime = Omega::instance().getSimulationTime();

	unsigned int sec	= (unsigned int)(simulationTime);
	unsigned int min	= sec/60;
	Real time		= (simulationTime-sec)*1000;
	unsigned int msec	= (unsigned int)(time);
	time			= (time-msec)*1000;
	unsigned int misec	= (unsigned int)(time);
	time			= (time-misec)*1000;
	unsigned int nsec	= (unsigned int)(time);
	sec			= sec-60*min;

	char strVirt[64];
	snprintf(strVirt,64,"virt %02d:%03d.%03dm%03du%03dn",min,sec,msec,misec,nsec);
	labelSimulTime->setText(string(strVirt));

	string sim=Omega::instance().getSimulationFileName();
	tlCurrentSimulation->setText(sim.empty() ? "[no file]" : sim); 

	if(Omega::instance().isRunning()){
		duration = microsec_clock::local_time()-Omega::instance().getMsStartingSimulationTime();
		duration -= Omega::instance().getSimulationPauseDuration();

		unsigned int hours	= duration.hours();
		unsigned int minutes 	= duration.minutes();
		unsigned int seconds	= duration.seconds();
		unsigned int mseconds	= duration.fractional_seconds()/1000;
		unsigned int days 	= hours/24;
		hours			= hours-24*days;

		char strReal[64];
		if(days>0) snprintf(strReal,64,"real %dd %02d:%02d:%03d.%03d",days,hours,minutes,seconds,mseconds);
		else snprintf(strReal,64,"real %02d:%02d:%03d.%03d",hours,minutes,seconds,mseconds);
		labelRealTime->setText(string(strReal));
        
	}
	// update iterations per second - only one in a while (iterPerSec_TTL_ms)
	// does someone need to display that with more precision than integer?
	long iterPerSec_LastAgo_ms=(microsec_clock::local_time()-iterPerSec_LastLocalTime).total_milliseconds();
	if(iterPerSec_LastAgo_ms>iterPerSec_TTL_ms){
		iterPerSec=(1000.*(Omega::instance().getCurrentIteration()-iterPerSec_LastIter))/iterPerSec_LastAgo_ms;
		iterPerSec_LastIter=Omega::instance().getCurrentIteration();
		iterPerSec_LastLocalTime=microsec_clock::local_time();
	}
	char strIter[64];
	/* print 0 instead of bogus values (at startup) */
	snprintf(strIter,64,"iter #%ld, %.1f/s",Omega::instance().getCurrentIteration(),(iterPerSec<1e9 && iterPerSec>0)?iterPerSec:0.);
	labelIter->setText(strIter);

   // update estimation time
	char strEstimation[64];
	if (Omega::instance().getRootBody()->stopAtIteration>0 && iterPerSec>0 ) estimation=duration+time_duration(seconds((Omega::instance().getRootBody()->stopAtIteration-Omega::instance().getCurrentIteration())/iterPerSec));
	snprintf(strEstimation,64,"estimation %02d:%02d:%02d",estimation.hours(),estimation.minutes(),estimation.seconds());
 	labelEstimationTime->setText(strEstimation);

	char strStopAtIter[64];
	snprintf(strStopAtIter,64,"stopAtIter #%ld",Omega::instance().getRootBody()->stopAtIteration);
	labelStopAtIter->setText(strStopAtIter);

	if(sbRefreshTime->value()!=refreshTime) sbRefreshTime->setValue(refreshTime);

	char strStep[64];
	snprintf(strStep,64,"step %g",(double)Omega::instance().getTimeStep());
	labelStep->setText(string(strStep));

	//changeSkipTimeStepper = false;
	//changeTimeStep = false;

	//cerr<<"dt="<<dt<<",exp10="<<exp10<<",10^exp10="<<pow((float)10,exp10)<<endl;

	/* enable/disable controls here, dynamically */
	hasSimulation=(Omega::instance().getRootBody() ? Omega::instance().getRootBody()->bodies->size()>0 : false );
	bool	isRunning=Omega::instance().isRunning() || syncRunning,
		hasTimeStepper=Omega::instance().containTimeStepper(),
		usesTimeStepper=Omega::instance().timeStepperActive(),
		hasFileName=(Omega::instance().getSimulationFileName()!="");

	pbStartSimulation->setEnabled(hasSimulation && !isRunning);
	pbStopSimulation->setEnabled(hasSimulation && isRunning);
	pbResetSimulation->setEnabled(hasSimulation && hasFileName);
	pbOneSimulationStep->setEnabled(hasSimulation && !isRunning);
	rbFixed->setEnabled(true);
	rbTimeStepper->setEnabled(hasTimeStepper);
	sbSecond->setEnabled(!usesTimeStepper);
	sb10PowerSecond->setEnabled(!usesTimeStepper);

	// conditionals only avoid setting the state that is already set, to avoid spurious signals
	if(rbFixed->isChecked()==usesTimeStepper){ LOG_DEBUG("Checking rbFixed"); rbFixed->setChecked(!usesTimeStepper); }
	if(rbTimeStepper->isChecked()!=usesTimeStepper){ LOG_DEBUG("Checking rbTimeStepper"); rbTimeStepper->setChecked(usesTimeStepper); }

	int exp10,mantissa; dtIntegerMantissaExponent(mantissa,exp10);
	sb10PowerSecond->setValue(exp10); sbSecond->setValue(mantissa);

}

void SimulationController::dtIntegerMantissaExponent(int& mantissa, int& exponent){
	Real dt=Omega::instance().getTimeStep();
	exponent=floor(log10(dt));
	mantissa=((int)(.1+dt/(pow((float)10,exponent)))); // .1: rounding issues
}
