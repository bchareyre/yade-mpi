/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include <boost/thread/mutex.hpp>

#ifndef SIMULATION_RUNNER_HPP
#define SIMULATION_RUNNER_HPP

/*! 
\brief	ThreadRunner takes care of starting/stopping (executing) the
	simulation flow in the separate thread. 

	Also it can execute a single loop of simulation flow in a separate
	thread.

	Lifetime of ThreadRunner is guaranteed to be longer or equal to
	the lifetime of	the separate thread.

	Lifetime of ThreadRunner must be ensured by the user to be shorter
	or equal to the lifetime of ThreadWorker.

	Do not destroy immediately after call to singleLoop(). Destructor can
	kick in before a separate thread starts, which will lead to a crash.


\note	This code is reentrant. Simultaneous requests from other threads to
	start/stop or perform singleLoop are expected.
	   
	So simulations are running, while the user is interacting with the
	UI frontend (doesn't matter whether the UI is graphical, ncurses or
	any other).

 */

class ThreadWorker;

class ThreadRunner
{
	private :
		ThreadWorker* simulationFlow_;
		bool		running_;
		boost::mutex	boolmutex_;
		boost::mutex	callmutex_;
		boost::mutex	runmutex_;
		void		run();
		void		call();

	public :
		ThreadRunner(ThreadWorker* c) : simulationFlow_(c), running_(false) {};
		~ThreadRunner();

		void singleLoop();
		void start();
		void stop();
		bool isRunning();
};

#endif // SIMULATIONLOOP_HPP

