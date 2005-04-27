/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OMEGA_H__
#define __OMEGA_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/date_time/posix_time/posix_time.hpp>
#include <fstream>
#include <time.h>
#include <boost/thread/thread.hpp>
#include <iostream>
#include <boost/shared_ptr.hpp>
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Vector3.hpp"
#include "Singleton.hpp"
#include "ClassFactory.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class MetaBody;
class SimulationLoop;
class FrontEnd;
class ThreadSynchronizer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace boost;
using namespace boost::posix_time;
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


class Omega : public Singleton<Omega>
{
	private	: shared_ptr<ofstream> logFile;
	public	: void logMessage(const string& str);
	public	: void logError(const string& str);
		
	private	: boost::mutex omegaMutex;
	//public	: boost::mutex& getOmegaMutex();

	private	: boost::mutex rootBodyMutex;
	public	: boost::mutex& getRootBodyMutex();
	
	private	: shared_ptr<SimulationLoop> simulationLoop;
	public	: void createSimulationLoop();
	public	: void startSimulationLoop();
	public	: void stopSimulationLoop();
	public	: void finishSimulationLoop();
	public  : void joinSimulationLoop();
	public  : void doOneSimulationLoop();
	
	private	: shared_ptr<ThreadSynchronizer> synchronizer;
	public  : shared_ptr<ThreadSynchronizer> getSynchronizer();

	private : map<string,string> dynlibsType;
	public  : const map<string,string>& getDynlibsType();	
	public	: bool getDynlibType(const string& libName,string& type);
	private	: void buildDynlibList();
	private	: void registerDynlibType(const string& name);
	
	private	: Real dt; // FIXME - maybe ???????? move this to MetaBody.hpp OR MAYBE NOT ??
	public	: void setTimeStep(const Real);
	public	: Real getTimeStep();
	
	private	: shared_ptr<MetaBody> rootBody;
	public	: const shared_ptr<MetaBody>& getRootBody();
	public  : void freeRootBody();
	
	private	: ptime	sStartingSimulationTime;
	private	: ptime msStartingSimulationTime;
	private	: ptime msStartingPauseTime;
	private : time_duration simulationPauseDuration;
	public  : ptime getMsStartingSimulationTime();
	public  : time_duration getSimulationPauseDuration();
	
	private	: string simulationFileName;
	public	: void setSimulationFileName(const string);
	public	: string getSimulationFileName();
	public	: void loadSimulation();

	private : long int currentIteration;
	public  : long int getCurrentIteration();
	public  : void incrementCurrentIteration();
	
	private : Real simulationTime;
	public	: Real getSimulationTime() { return simulationTime;};
	public	: void incrementSimulationTime() { simulationTime+=dt;};
	
	private	: Omega();
	private	: ~Omega();
	private	: Omega(const Omega&);
	private	: Omega& operator=(const Omega&);
	
	FRIEND_SINGLETON(Omega);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


#endif // __OMEGA_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
