/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include<yade/core/Omega.hpp>
#include "SimulationControllerUpdater.hpp"
#include "SimulationController.hpp"
#include<qlabel.h>
#include <qspinbox.h>
#include<cstdio>

CREATE_LOGGER(SimulationControllerUpdater);

SimulationControllerUpdater::SimulationControllerUpdater(SimulationController * sc)
        :       controller(sc), iterPerSec_TTL_ms(1000)
{
	iterPerSec_LastIter=Omega::instance().getCurrentIteration();
	iterPerSec_LastLocalTime=microsec_clock::local_time();
}


SimulationControllerUpdater::~SimulationControllerUpdater()
{

}

void SimulationControllerUpdater::oneLoop()
{
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
	controller->labelSimulTime->setText(string(strVirt));

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
	controller->labelRealTime->setText(string(strReal));

	// update iterations per second - only one in a while (iterPerSec_TTL_ms)
	// does someone need to display that with more precision than integer?
	long iterPerSec_LastAgo_ms=(microsec_clock::local_time()-iterPerSec_LastLocalTime).total_milliseconds();
	if(iterPerSec_LastAgo_ms>iterPerSec_TTL_ms){
		iterPerSec=(1000*(Omega::instance().getCurrentIteration()-iterPerSec_LastIter))/iterPerSec_LastAgo_ms;
		//controller->tlIterPerSec->setText(lexical_cast<string>(iterPerSec));
		iterPerSec_LastIter=Omega::instance().getCurrentIteration();
		iterPerSec_LastLocalTime=microsec_clock::local_time();
	}

	char strIter[64];
	snprintf(strIter,64,"iter #%ld, %.1f/s",Omega::instance().getCurrentIteration(),iterPerSec);
	controller->labelIter->setText(strIter);

	// FIXME - why updater is controlling the timestep ??
	if (controller->changeSkipTimeStepper)
			Omega::instance().skipTimeStepper(controller->skipTimeStepper);

	if (controller->changeTimeStep)
	{
		Real second = (Real)(controller->sbSecond->value());
		Real powerSecond = (Real)(controller->sb10PowerSecond->value());
		Omega::instance().setTimeStep(second*Mathr::Pow(10,powerSecond));
	}

	//string strDt = lexical_cast<string>(Omega::instance().getTimeStep());
	//controller->tlTimeStep->setText(strDt);

	char strStep[64];
	snprintf(strStep,64,"step %g",Omega::instance().getTimeStep());
	controller->labelStep->setText(string(strStep));

	controller->changeSkipTimeStepper = false;
	controller->changeTimeStep = false;
}


