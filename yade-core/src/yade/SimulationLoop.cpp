/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "SimulationLoop.hpp"
#include "MetaBody.hpp"
#include "Omega.hpp"
//#include <yade/yade-lib-threads/ThreadSynchronizer.hpp>

SimulationLoop::SimulationLoop()// : Threadable<SimulationLoop>(Omega::instance().getSynchronizer())
{
//	createThread();
}


SimulationLoop::~SimulationLoop()
{

}

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

//bool running=false;
boost::mutex mut,runmutex;

class Running
{
	private:
		bool	running_;
	public:
		Running() : running_(false) {}
		bool val()
		{
			boost::mutex::scoped_lock lock(runmutex);
			return running_;
		};
		void tru()
		{
			boost::mutex::scoped_lock lock(runmutex);
			running_=true;
		};
		void fal()
		{
			boost::mutex::scoped_lock lock(runmutex);
			running_=false;
		};

};

Running running;


bool SimulationLoop::isStopped()
{
	boost::mutex::scoped_lock lock(mut);
	return !running.val();
}

void run()
{
	while(running.val())
		Omega::instance().doOneSimulationLoop();
}


void SimulationLoop::doOneLoop()
{
//	LOCK(Omega::instance().getRootBodyMutex());
		
	boost::mutex::scoped_lock lock(mut);
	if (Omega::instance().getRootBody())
	{
		Omega::instance().getRootBody()->moveToNextTimeStep();
		Omega::instance().incrementCurrentIteration();
		Omega::instance().incrementSimulationTime();
	}
}


void SimulationLoop::start()
{
	boost::mutex::scoped_lock lock(mut);
	running.tru();
	boost::thread th(&run);
	std::cerr << "start\n";
}

void SimulationLoop::stop()
{
	boost::mutex::scoped_lock lock(mut);
	running.fal();
	std::cerr << "stop\n";
}

