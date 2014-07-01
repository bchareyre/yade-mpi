/*************************************************************************
* Copyright (C) 2004 by Olivier Galizzi         *
* olivier.galizzi@imag.fr            *
* Copyright (C) 2004 by Janek Kozicki         *
* cosurgi@berlios.de             *
*                  *
* This program is free software; it is licensed under the terms of the *
* GNU General Public License v2 or later. See file LICENSE for details. *
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

#include<yade/lib/base/Math.hpp>
#include<yade/lib/factory/ClassFactory.hpp>

#include<yade/lib/base/Singleton.hpp>

#include "SimulationFlow.hpp"


#ifndef FOREACH
# define FOREACH BOOST_FOREACH
#endif

class Scene;
class ThreadRunner;

using namespace boost::posix_time;
using std::string;
using std::vector;
using std::runtime_error;
using std::endl;
using std::cout;
using std::cerr;

struct DynlibDescriptor{
	std::set<string> baseClasses;
	bool isIndexable, isFactorable, isSerializable;
};

class Omega: public Singleton<Omega>{
	shared_ptr<ThreadRunner> simulationLoop;
	SimulationFlow simulationFlow_;
	std::map<string,DynlibDescriptor> dynlibs; // FIXME : should store that in ClassFactory ?
	void buildDynlibDatabase(const vector<string>& dynlibsList); // FIXME - maybe in ClassFactory ?
	
	vector<shared_ptr<Scene> > scenes;
	int currentSceneNb;
	shared_ptr<Scene> sceneAnother; // used for temporarily running different simulation, in Omega().switchscene()

	ptime startupLocalTime;

	std::map<string,string> memSavedSimulations;

	// to avoid accessing simulation when it is being loaded (should avoid crashes with the UI)
	boost::mutex loadingSimulationMutex;
	boost::mutex tmpFileCounterMutex;
	long tmpFileCounter;
	std::string tmpFileDir;

	public:
		// management, not generally useful
		void init();
		void reset();
		void timeInit();
		void initTemps();
		void cleanupTemps();
		const std::map<string,DynlibDescriptor>& getDynlibsDescriptor();
		void loadPlugins(vector<string> pluginFiles);
		bool isInheritingFrom(const string& className, const string& baseClassName );
		bool isInheritingFrom_recursive(const string& className, const string& baseClassName );
		void createSimulationLoop();
		bool hasSimulationLoop(){return (bool)(simulationLoop);}
		string gdbCrashBatch;
		char** origArgv; int origArgc;
		// do not change by hand
		/* Mutex for:
		* 1. GLViewer::paintGL (deffered lock: if fails, no GL painting is done)
		* 2. other threads that wish to manipulate GL
		* 3. Omega when substantial changes to the scene are being made (bodies being deleted, simulation loaded etc) so that GL doesn't access those and crash */
		boost::try_mutex renderMutex;


		void run();
		void pause();
		void step();
		void stop(); // resets the simulationLoop
		bool isRunning();
		std::string sceneFile; // updated at load/save automatically
		void loadSimulation(const string& name, bool quiet=false);
		void saveSimulation(const string& name, bool quiet=false);

		void resetScene();
		void resetCurrentScene();
		void resetAllScenes();
		const shared_ptr<Scene>& getScene();
		int addScene();
		void switchToScene(int i);
		//! Return unique temporary filename. May be deleted by the user; if not, will be deleted at shutdown.
		string tmpFilename();
		Real getRealTime();
		time_duration getRealTime_duration();

		// configuration directory used for logging config and possibly other things
		std::string confDir;

	DECLARE_LOGGER;

	Omega(){ LOG_DEBUG("Constructing Omega."); }
	~Omega(){}

	FRIEND_SINGLETON(Omega);
	friend class pyOmega;
};


