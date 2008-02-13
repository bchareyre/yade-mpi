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
#include <qlcdnumber.h>
#include <qspinbox.h>

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
	controller->lcdCurrentIteration->display(lexical_cast<string>(Omega::instance().getCurrentIteration()));
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

	controller->lcdMinutev->display(lexical_cast<string>(min));
	controller->lcdSecondv->display(lexical_cast<string>(sec));
	controller->lcdMSecondv->display(lexical_cast<string>(msec));
	controller->lcdMiSecondv->display(lexical_cast<string>(misec));
	controller->lcdNSecondv->display(lexical_cast<string>(nsec));

	time_duration duration = microsec_clock::local_time()-Omega::instance().getMsStartingSimulationTime();
	duration -= Omega::instance().getSimulationPauseDuration();

	unsigned int hours	= duration.hours();
	unsigned int minutes 	= duration.minutes();
	unsigned int seconds	= duration.seconds();
	unsigned int mseconds	= duration.fractional_seconds()/1000;
	unsigned int days 	= hours/24;
	hours			= hours-24*days;

	controller->lcdDay->display(lexical_cast<string>(days));
	controller->lcdHour->display(lexical_cast<string>(hours));
	controller->lcdMinute->display(lexical_cast<string>(minutes));
	controller->lcdSecond->display(lexical_cast<string>(seconds));
	controller->lcdMSecond->display(lexical_cast<string>(mseconds));

	// update iterations per second - only one in a while (iterPerSec_TTL_ms)
	// does someone need to display that with more precision than integer?
	long iterPerSec_LastAgo_ms=(microsec_clock::local_time()-iterPerSec_LastLocalTime).total_milliseconds();
	if(iterPerSec_LastAgo_ms>iterPerSec_TTL_ms){
		controller->tlIterPerSec->setText(lexical_cast<string>((1000*(Omega::instance().getCurrentIteration()-iterPerSec_LastIter))/iterPerSec_LastAgo_ms));
		iterPerSec_LastIter=Omega::instance().getCurrentIteration();
		iterPerSec_LastLocalTime=microsec_clock::local_time();
	}

	// FIXME - why updater is controlling the timestep ??
	if (controller->changeSkipTimeStepper)
			Omega::instance().skipTimeStepper(controller->skipTimeStepper);

	if (controller->changeTimeStep)
	{
		Real second = (Real)(controller->sbSecond->value());
		Real powerSecond = (Real)(controller->sb10PowerSecond->value());
		Omega::instance().setTimeStep(second*Mathr::Pow(10,powerSecond));
	}

// 	Real dt = Omega::instance().getTimeStep();
// 	int i=0;
// 	while (dt<1)
// 	{
// 		dt *=10;
// 		i++;
// 	}
// 
// 	controller->lcdDtSecond->display((int)dt);
// 
// 	dt = dt-(int)dt;
// 	while (dt-(int)dt!=0)
// 		dt*=10;
// 
// 	controller->lcdDt10PowerSecond->display((int)dt);
	string strDt = lexical_cast<string>(Omega::instance().getTimeStep());
	controller->tlTimeStep->setText(strDt);


	controller->changeSkipTimeStepper = false;
	controller->changeTimeStep = false;
}


