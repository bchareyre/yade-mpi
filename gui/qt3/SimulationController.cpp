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
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <unistd.h>
#include<yade/core/yadeExceptions.hpp>
#include <Wm3Math.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<boost/foreach.hpp>
#	ifndef FOREACH
#		define FOREACH BOOST_FOREACH
#	endif

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

	iterPerSec_LastIter=Omega::instance().getCurrentIteration();
	iterPerSec_LastLocalTime=microsec_clock::local_time();

	parentWorkspace = parent;

	scrollViewFrame = new QFrame();	
	
	scrollViewLayout = new QVBoxLayout( scrollViewOutsideFrame, 0, 0, "scrollViewLayout"); 
	
	scrollView = new QScrollView( scrollViewOutsideFrame, "scrollView" );
	scrollView->setVScrollBarMode(QScrollView::Auto);
	scrollView->setHScrollBarMode(QScrollView::Auto);
	scrollViewLayout->addWidget( scrollView );
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
	addNewView();

	// there is file existence assertion in lodSimulationFromFilename, so yade will abort cleanly...
	LOG_DEBUG("Omega's simulation filename: `"<<Omega::instance().getSimulationFileName()<<"'");
	if (Omega::instance().getSimulationFileName()!="" && (!Omega::instance().getRootBody() || (Omega::instance().getRootBody()->bodies->size()==0 && Omega::instance().getRootBody()->engines.size()==0))){
		loadSimulationFromFileName(Omega::instance().getSimulationFileName());
	}
	// run timer ANY TIME (simulation may be started asynchronously)
	updateTimerId=startTimer(refreshTime);
}

/* restart timer with SimulationController::refreshTime */
void SimulationController::restartTimer(){ killTimer(updateTimerId); updateTimerId=startTimer(refreshTime); }


void SimulationController::pbApplyClicked()
{
	guiGen.deserialize(YadeQtMainWindow::self->renderer);
}


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


void SimulationController::loadSimulationFromFileName(const std::string& fileName,bool center, bool useTimeStepperIfPresent)
{
	assert(filesystem::exists(fileName));

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

			Real dt=Omega::instance().getTimeStep();
			int exp10=(int)floor(log10(dt));
			sbSecond->setValue((int)(dt/(pow(10.,exp10)))); // we may lose quite some precision here :-(
			sb10PowerSecond->setValue(exp10);

			changeSkipTimeStepper = true;
			if (Omega::instance().containTimeStepper())
			{
				rbTimeStepper->setEnabled(true);
				rbTimeStepper->setChecked(useTimeStepperIfPresent);
				wasUsingTimeStepper = useTimeStepperIfPresent;
			}
			else
			{
				rbTimeStepper->setEnabled(false);
				rbFixed->setChecked(true);
				wasUsingTimeStepper = false;
			}
			skipTimeStepper=!wasUsingTimeStepper;
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
		catch(yadeError& e)
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
		shared_ptr<MessageDialog> md = shared_ptr<MessageDialog>(new MessageDialog("Save aborted (check file extension).",this->parentWidget()->parentWidget()));
		md->exec(); 
	}
}

void SimulationController::pbNewViewClicked() {	addNewView(); }
void SimulationController::addNewView(){ YadeQtMainWindow::self->createView(); }
void SimulationController::pbCenterSceneClicked() { YadeQtMainWindow::self->centerViews(); }
void SimulationController::closeEvent(QCloseEvent *){ /* switch to async run if running */ if(syncRunning) cbSyncToggled(false); emit closeSignal(); }
void SimulationController::pbStopClicked() { Omega::instance().stopSimulationLoop(); syncRunning=false; }
void SimulationController::pbOneSimulationStepClicked(){pbStopClicked();Omega::instance().spawnSingleSimulationLoop();}
void SimulationController::pbStart2Clicked() { pbStartClicked(); }
void SimulationController::pbStartClicked(){
	restartTimer();
	if(sync) syncRunning=true;
	else Omega::instance().startSimulationLoop();
}

void SimulationController::cbSyncToggled(bool b){	sync=b; if(sync && refreshTime<20) refreshTime=20; pbStopClicked(); pbStartClicked(); }
void SimulationController::timerEvent( QTimerEvent* ){
	doUpdate(); /* update the controller, like iteration number etc */
	if(sync && syncRunning){
		/* update GLViews */
		//LOG_DEBUG("Sync: iter "<<Omega::instance().getCurrentIteration());
		YadeQtMainWindow::self->redrawAll(true);
		Omega::instance().spawnSingleSimulationLoop();
	}
}

void SimulationController::pbResetClicked()
{
	pbStopClicked();

	std::string name=Omega::instance().getSimulationFileName(); 
	loadSimulationFromFileName(name,false /* don't re-center scene */,wasUsingTimeStepper /* respect timeStepper setting from the prvious run*/);

	if(Omega::instance().getRootBody())
	{
		// timeStepper setup done in loadSimulationFromFileName
	} 
	else
	{
		Omega::instance().setSimulationFileName(name);
		pbResetSimulation->setEnabled(true);
	}

//	pbCenterSceneClicked();  // FIXME - add autocenter option...
}






void SimulationController::bgTimeStepClicked(int i)
{
	// cerr<<"CHANGING TIME STEP: i="<<i<<endl;
	/* i: buttonGroupId, which is 0 for timeStepper, 2 for fixed step */
	switch (i)
	{
		case 0 : //Use timeStepper
			changeSkipTimeStepper = true;
			skipTimeStepper = false;
			wasUsingTimeStepper=true;
			break;
		case 1 : // Try RealTime -- deprecated
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
	if(!rbFixed->isOn()){ rbFixed->toggle(); bgTimeStepClicked(2); } // this should do the callback as if user clicked fixed timestepper button
	changeTimeStep = true;
}


void SimulationController::sbSecondValueChanged(int)
{ 
	if(!rbFixed->isOn()){ rbFixed->toggle(); bgTimeStepClicked(2); } // dtto
	changeTimeStep = true;
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


void SimulationController::doUpdate(){

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

	if(Omega::instance().isRunning()){
		time_duration duration = microsec_clock::local_time()-Omega::instance().getMsStartingSimulationTime();
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

	if (changeSkipTimeStepper) Omega::instance().skipTimeStepper(skipTimeStepper);
	if (changeTimeStep) {
		// wrap the mantissa around
		if(sbSecond->value()==0){ sbSecond->setValue(9); sb10PowerSecond->setValue(sb10PowerSecond->value()-1); }
		if(sbSecond->value()==10){ sbSecond->setValue(1); sb10PowerSecond->setValue(sb10PowerSecond->value()+1); }
		Real second = (Real)(sbSecond->value());
		Real powerSecond = (Real)(sb10PowerSecond->value());
		Omega::instance().setTimeStep(second*Mathr::Pow(10,powerSecond));
	} else {
		Real dt=Omega::instance().getTimeStep();
		int exp10=floor(log10(dt));
		sb10PowerSecond->setValue(exp10);
		sbSecond->setValue((int)(.1+dt/(pow((float)10,exp10)))); // .1: rounding issues
	}

	if(sbRefreshTime->value()!=refreshTime) sbRefreshTime->setValue(refreshTime);

	char strStep[64];
	snprintf(strStep,64,"step %g",Omega::instance().getTimeStep());
	labelStep->setText(string(strStep));

	changeSkipTimeStepper = false;
	changeTimeStep = false;

	//cerr<<"dt="<<dt<<",exp10="<<exp10<<",10^exp10="<<pow((float)10,exp10)<<endl;

	/* enable/disable controls here, dynamically */
	bool hasSimulation=(Omega::instance().getRootBody() ? Omega::instance().getRootBody()->bodies->size()>0 : false ),
		isRunning=Omega::instance().isRunning() || syncRunning,
		hasTimeStepper=Omega::instance().containTimeStepper(),
		usesTimeStepper=Omega::instance().timeStepperActive(),
		hasFileName=(Omega::instance().getSimulationFileName()!="");

	pbStartSimulation->setEnabled(hasSimulation && !isRunning);
	pbStopSimulation->setEnabled(hasSimulation && isRunning);
	pbResetSimulation->setEnabled(hasSimulation && hasFileName);
	pbOneSimulationStep->setEnabled(hasSimulation && !isRunning);
	rbTimeStepper->setEnabled(hasTimeStepper);
	// conditionals only avoid setting the state that is already set, to avoid spurious signals
	if(rbFixed->isChecked()==usesTimeStepper) rbFixed->setChecked(!usesTimeStepper);
	if(rbTimeStepper->isChecked()!=usesTimeStepper) rbTimeStepper->setChecked(usesTimeStepper);

}

