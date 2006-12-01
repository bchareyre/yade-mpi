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

#include <boost/date_time/posix_time/posix_time.hpp>
#include <fstream>
#include <set>
#include <time.h>
#include <boost/thread/thread.hpp>
#include <iostream>
#include <boost/shared_ptr.hpp>

#include <yade/yade-lib-wm3-math/Vector3.hpp>
#include <yade/yade-lib-loki/Singleton.hpp>
#include <yade/yade-lib-factory/ClassFactory.hpp>

#include "SimulationFlow.hpp"

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
		shared_ptr<ofstream>		 logFile;
		boost::mutex			 omegaMutex
						,rootBodyMutex;

		shared_ptr<ThreadRunner>	 simulationLoop;
		SimulationFlow			 simulationFlow_;

		map<string,DynlibDescriptor>	 dynlibs;	// FIXME : should store that in ClassFactory ?
		Real				 dt;		// FIXME - maybe ? move this to MetaBody.hpp OR MAYBE NOT ?
		
		shared_ptr<MetaBody>		 rootBody;
		ptime				 sStartingSimulationTime;
		ptime				 msStartingSimulationTime;
		ptime				 msStartingPauseTime;
		time_duration			 simulationPauseDuration;
		string				 simulationFileName;
		long int			 currentIteration;
		Real				 simulationTime;
		
		
		void buildDynlibDatabase(const vector<string>& dynlibsList); // FIXME - maybe in ClassFactory ?

	public :
		shared_ptr<Preferences> preferences;
		string 				 yadeConfigPath;	// FIXME - must be private and more clean
		string 				 yadeVersionName;	// FIXME - public ?
	
		void logMessage(const string& str);
		void logError(const string& str);
		
		boost::mutex&	getRootBodyMutex();
		
		void		createSimulationLoop();
		void		startSimulationLoop();
		void		stopSimulationLoop();
		void		finishSimulationLoop();
		void		joinSimulationLoop();
		void		spawnSingleSimulationLoop();

        //      shared_ptr<ThreadSynchronizer> getSynchronizer();

		const		map<string,DynlibDescriptor>& getDynlibsDescriptor();
		void		scanPlugins();
		bool		isInheritingFrom(const string& className, const string& baseClassName );

		void		setTimeStep(const Real);
		Real		getTimeStep();
		void		skipTimeStepper(bool s);
		bool		containTimeStepper();

		const		shared_ptr<MetaBody>& getRootBody();
		void		freeRootBody();
		
		ptime		getMsStartingSimulationTime();
		time_duration	getSimulationPauseDuration();
		
		void		setSimulationFileName(const string);
		string		getSimulationFileName();
		void		loadSimulation();
		void		saveSimulation(const string name);

		long int	getCurrentIteration();
		void		incrementCurrentIteration();
		
		Real		getSimulationTime() { return simulationTime;};
		void		incrementSimulationTime() { simulationTime+=dt;};
		
		void		init();

		DECLARE_LOGGER;

	private	:
		Omega();
		~Omega();
		Omega(const Omega&);
		Omega& operator=(const Omega&);

	FRIEND_SINGLETON(Omega);
};

#endif // OMEGA_HPP

