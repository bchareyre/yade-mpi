/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "SimulationRunner.hpp"
#include "SimulationFlow.hpp"

#include <boost/thread/thread.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

void SimulationRunner::run()
{
	boost::mutex::scoped_lock lock(runmutex_);
	while(isRunning())
		call();
}

void SimulationRunner::call()
{
	boost::mutex::scoped_lock lock(callmutex_);
	simulationFlow_->singleLoop();
	// can add here a signal, that notifies the UI about single loop being completed
}

void SimulationRunner::singleLoop()
{
	boost::mutex::scoped_lock boollock(boolmutex_);
	boost::mutex::scoped_lock calllock(callmutex_);
	if(running_) return;
	boost::function0<void> call( boost::bind( &SimulationRunner::call , this ) );
	boost::thread th(call);
}

void SimulationRunner::start()
{
	boost::mutex::scoped_lock lock(boolmutex_);
	if(running_) return;
	running_=true;
	boost::function0<void> run( boost::bind( &SimulationRunner::run , this ) );
	boost::thread th(run);
}

void SimulationRunner::stop()
{
	boost::mutex::scoped_lock lock(boolmutex_);
	running_=false;
}

bool SimulationRunner::isRunning()
{
	boost::mutex::scoped_lock lock(boolmutex_);
	return running_;
}

SimulationRunner::~SimulationRunner()
{
	stop();
	boost::mutex::scoped_lock runlock(runmutex_);
	boost::mutex::scoped_lock calllock(callmutex_);
}

