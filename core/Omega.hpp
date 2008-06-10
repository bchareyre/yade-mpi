/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef OMEGA_HPP
#define OMEGA_HPP

// qt3 sucks
#ifdef EMBED_PYTHON
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
#endif

#include <boost/date_time/posix_time/posix_time.hpp>
#include <fstream>
#include <set>
#include <list>
#include <time.h>
#include <boost/thread/thread.hpp>
#include <iostream>
#include <boost/shared_ptr.hpp>

#include<Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/lib-loki/Singleton.hpp>
#include<yade/lib-factory/ClassFactory.hpp>


#include "SimulationFlow.hpp"
#include "Body.hpp"

#ifndef FOREACH
#	define FOREACH BOOST_FOREACH
#endif

class MetaBody;
class ThreadRunner;
class Preferences;

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

class Omega : public Singleton<Omega>
{

	private	:
		boost::mutex			 omegaMutex
						,rootBodyMutex;

		shared_ptr<ThreadRunner>	 simulationLoop;
		SimulationFlow			 simulationFlow_;

		map<string,DynlibDescriptor>	 dynlibs;	// FIXME : should store that in ClassFactory ?
		
		shared_ptr<MetaBody>		 rootBody;
		//ptime				 sStartingSimulationTime;
		ptime				 msStartingSimulationTime;
		ptime				 msStartingPauseTime;
		time_duration			 simulationPauseDuration;
		string				 simulationFileName;
		long int			currentIteration;
		Real				simulationTime;
		void buildDynlibDatabase(const vector<string>& dynlibsList); // FIXME - maybe in ClassFactory ?

		public: long int stopAtIteration;

	public :
		shared_ptr<Preferences> preferences;
		string 				 yadeConfigPath;	// FIXME - must be private and more clean
		string 				 yadeVersionName;	// FIXME - public ?
		list<body_id_t> selectedBodies;
	
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
		
		boost::mutex&	getRootBodyMutex();
		
		void		createSimulationLoop();
		bool 		hasSimulationLoop(){return (bool)(simulationLoop);}
		void		startSimulationLoop();
		void		stopSimulationLoop();
		void		finishSimulationLoop();
		void		joinSimulationLoop();
		void		spawnSingleSimulationLoop();
		bool		isRunning();
        //      shared_ptr<ThreadSynchronizer> getSynchronizer();

		const		map<string,DynlibDescriptor>& getDynlibsDescriptor();
		void		scanPlugins();
		bool		isInheritingFrom(const string& className, const string& baseClassName );

		void		setTimeStep(const Real);
		Real		getTimeStep();
		void		skipTimeStepper(bool s);
		bool 		timeStepperActive();
		bool		containTimeStepper();

		const		shared_ptr<MetaBody>& getRootBody();
		void		setRootBody(shared_ptr<MetaBody>&);
		void		resetRootBody();
		
		ptime		getMsStartingSimulationTime();
		time_duration	getSimulationPauseDuration();
		
		void		setSimulationFileName(const string);
		string	getSimulationFileName();
		void		loadSimulation();
		void		saveSimulation(const string name, bool recover=false);

		long int	getCurrentIteration();
		void		incrementCurrentIteration();
		
		Real		getSimulationTime() { return simulationTime;};
		void		incrementSimulationTime() { simulationTime+=getTimeStep();};
		
		void init();
		void timeInit();

		void reset();

		DECLARE_LOGGER;

	private	:
		Omega();
		~Omega();
		Omega(const Omega&);
		Omega& operator=(const Omega&);

	FRIEND_SINGLETON(Omega);
};

#endif // OMEGA_HPP

