/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ThreadRunner.hpp"
#include "ThreadWorker.hpp"

#include <boost/thread/thread.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

void ThreadRunner::run()
{
	boost::mutex::scoped_lock lock(runmutex_);
	while(isRunning())
		call();
}

void ThreadRunner::call()
{
	boost::mutex::scoped_lock lock(callmutex_);
	m_thread_worker->setTerminate(false);
	m_thread_worker->callSingleAction();
	// can add here a signal, that notifies the UI about single loop being completed
}

void ThreadRunner::pleaseTerminate()
{
	stop();
	m_thread_worker->setTerminate(true);
}

void ThreadRunner::spawnSingleAction()
{
	boost::mutex::scoped_lock boollock(boolmutex_);
	boost::mutex::scoped_lock calllock(callmutex_);
	if(running_) return;
	boost::function0<void> call( boost::bind( &ThreadRunner::call , this ) );
	boost::thread th(call);
}

void ThreadRunner::start()
{
	boost::mutex::scoped_lock lock(boolmutex_);
	if(running_) return;
	running_=true;
	boost::function0<void> run( boost::bind( &ThreadRunner::run , this ) );
	boost::thread th(run);
}

void ThreadRunner::stop()
{
	boost::mutex::scoped_lock lock(boolmutex_);
	running_=false;
}

bool ThreadRunner::isRunning()
{
	boost::mutex::scoped_lock lock(boolmutex_);
	return running_;
}

ThreadRunner::~ThreadRunner()
{
	pleaseTerminate();
	boost::mutex::scoped_lock runlock(runmutex_);
	boost::mutex::scoped_lock calllock(callmutex_);
}

