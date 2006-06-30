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
\brief	SimulationRunner takes care of starting/stopping (executing) the
	simulation flow in the separate thread. 

	Also it can execute a single loop of simulation flow in a separate
	thread.


\note	This code must be reentrant. Simultaneous requests from other
	threads to start/stop or perform singleLoop are expected.
	   
	So simulations are running, while the user is interacting with the
	UI frontend (doesn't matter whether the UI is graphical, ncurses or
	any other).

 */

class SimulationFlow;

class SimulationRunner
{
	private :
		SimulationFlow* simulationFlow_;// this assumes that class responsible
						// for the lifetime of calculationFlow_ is also
						// responsible for lifetime of SimulationRunner.
						//
						// is this assumption a good decision ?
						//
						// if SimulationFlow was to be destroyed,
						// then SimulationRunner must be destroyed
						// with it. How to enforce that ?
		bool		running_;
		boost::mutex	boolmutex_;
		boost::mutex	callmutex_;
		boost::mutex	runmutex_;
		void		run();
		void		call();

	public :
		SimulationRunner(SimulationFlow* c) : simulationFlow_(c), running_(false) {};
		~SimulationRunner();

		void singleLoop();
		void start();
		void stop();
		bool isRunning();
};

#endif // SIMULATIONLOOP_HPP

