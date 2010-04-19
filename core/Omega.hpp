/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

// qt3 sucks
#ifdef QT_MOC_CPP
	#undef slots
	#include<Python.h>
	#define slots slots
#else
	#ifdef slots
		#undef slots
		#include<Python.h>
		#define slots
	#else
		#include<Python.h>
	#endif
#endif

#include <boost/date_time/posix_time/posix_time.hpp>
#include <fstream>
#include <set>
#include <list>
#include <time.h>
#include <boost/thread/thread.hpp>
#include <iostream>

#include<yade/lib-base/Math.hpp>
#include<yade/lib-loki/Singleton.hpp>
#include<yade/lib-factory/ClassFactory.hpp>

#include "SimulationFlow.hpp"


#ifndef FOREACH
#	define FOREACH BOOST_FOREACH
#endif

class Scene;
class ThreadRunner;

using namespace boost;
using namespace boost::posix_time;
using namespace std;

struct DynlibDescriptor
{
	set<string>	 baseClasses;

	bool		 isIndexable
			,isFactorable
			,isSerializable;
};

class Omega: public Singleton<Omega>
{

	private:
		shared_ptr<ThreadRunner>	 simulationLoop;
		SimulationFlow			 simulationFlow_;

		map<string,DynlibDescriptor>	 dynlibs;	// FIXME : should store that in ClassFactory ?
		
		shared_ptr<Scene>	scene;

		shared_ptr<Scene>	sceneAnother; // used for temporarily running different simulation, in Omega().switchscene()

		//ptime				 sStartingSimulationTime;
		ptime				 msStartingSimulationTime;
		ptime				 msStartingPauseTime;
		time_duration			 simulationPauseDuration;
		string				 simulationFileName;

		void buildDynlibDatabase(const vector<string>& dynlibsList); // FIXME - maybe in ClassFactory ?

		map<string,string> memSavedSimulations;

		boost::mutex tmpFileCounterMutex;
		long tmpFileCounter;
		std::string tmpFileDir;

		// to avoid accessing simulation when it is being loaded (should avoid crashes with the UI)
		boost::mutex loadingSimulationMutex;

	public :
		string 				 yadeConfigPath;	// FIXME - must be private and more clean
		string 				 yadeVersionName;	// FIXME - public ?

		// FIXME this is a hack. See  GLViewer:86
		// problem is that currently there is no way to transmit arguments between UI and GLDraw* methods.
		// Omega will be deleted anyway, so, uh.. I am polluting it now :/
		float	isoValue;
		float	isoThick;
		int     isoSec;
		// dtto for gdb
		string gdbCrashBatch;
		string recoveryFilename;
		char** origArgv; int origArgc;

		// FIXME end
		
		/* Mutex for:
		 * 1. GLViewer::paintGL (deffered lock: if fails, no GL painting is done)
		 * 2. other threads that wish to manipulate GL
		 * 3. Omega when substantial changes to the scene are being made (bodies being deleted, simulation loaded etc) so that GL doesn't access those and crash
		 */
		boost::try_mutex renderMutex;
		
		void		createSimulationLoop();
		bool 		hasSimulationLoop(){return (bool)(simulationLoop);}
		void		startSimulationLoop();
		void		stopSimulationLoop();
		void		finishSimulationLoop();
		void		joinSimulationLoop();
		void		spawnSingleSimulationLoop();
		bool		isRunning();

		const		map<string,DynlibDescriptor>& getDynlibsDescriptor();
		void		scanPlugins(vector<string> baseDirs);
		void		loadPlugins(vector<string> pluginFiles);
		bool		isInheritingFrom(const string& className, const string& baseClassName );
		bool		isInheritingFrom_recursive(const string& className, const string& baseClassName );

		void		setTimeStep(const Real);
		Real		getTimeStep();
		void		skipTimeStepper(bool s);
		bool 		timeStepperActive();
		bool		containTimeStepper();

		const		shared_ptr<Scene>& getScene();
		void		setScene(shared_ptr<Scene>&);
		void		resetScene();
		
		ptime		getMsStartingSimulationTime();
		time_duration	getSimulationPauseDuration();
		
		void		setSimulationFileName(const string);
		string	getSimulationFileName();
		void		loadSimulation();
		void		saveSimulation(const string name);
		void 		saveSimulationToStream(std::ostream&);
		void 		loadSimulationFromStream(std::istream&);

		long int	getCurrentIteration();
		void		setCurrentIteration(long int i);
		
		Real		getSimulationTime();
		Real 		getComputationTime();
		time_duration getComputationDuration();
		
		void init();
		void timeInit();

		void initTemps();
		void cleanupTemps();
		//! Return unique temporary filename. May be deleted by the user; if not, will be deleted at shutdown.
		string tmpFilename();

		void reset();

		DECLARE_LOGGER;

		Omega(){ LOG_DEBUG("Constructing Omega."); }
		~Omega(){}

	FRIEND_SINGLETON(Omega);
	friend class pyOmega;
};


