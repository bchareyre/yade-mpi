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

class NonConnexBody;
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
	public	: shared_ptr<FrontEnd> gui;

	//private : shared_ptr<boost::thread> simulationThread;
	private	: shared_ptr<SimulationLoop> simulationLoop;
	public	: void createSimulationLoop();
	public	: void startSimulationLoop();
	public	: void stopSimulationLoop();
	public	: void finishSimulationLoop();
	public : void joinSimulationLoop();
	public	: shared_ptr<ThreadSynchronizer> synchronizer; // FIXME put private + add function waitMyTuren and finishedMyTurn

//	public : void waitMyTurn(int id); // FIXME put private + add function waitMyTuren and finishedMyTurn
//	public : void endMyTurn(); // FIXME put private + add function waitMyTuren and finishedMyTurn
//	public : void waitForSimulationEnd(); // FIXME put private + add function waitMyTuren and finishedMyTurn
//	public : int getNewTurnId();

	public	: map<string,string> dynlibsType;

	public	: bool getDynlibType(const string& libName,string& type);

	public	: shared_ptr<ofstream> logFile;

	private : Vector3r	gravity; // FIXME
	public	: Vector3r	getGravity();
	public	: void setGravity(Vector3r g);
	public	: double 	dt; // FIXME
	public	: void 		setTimeStep(const double);
	public	: double 	getTimeStep();
	
	public	: shared_ptr<NonConnexBody> rootBody;
	public	: ptime		sStartingSimulationTime;
	public	: ptime		msStartingSimulationTime;
	private	: void		buildDynlibList();
	private	: void		registerDynlibType(const string& name);

	private	: string 	simulationFileName;
	public	: void 		setSimulationFileName(const string);
	public	: string 	getSimulationFileName();
	public	: void		loadSimulation();

	public  : long int	currentIteration;
	public  : long int	getCurrentIteration();
	public  : void		incrementCurrentIteration();
	
	private : double	simulationTime;
	public	: double	getSimulationTime() { return simulationTime;};
	public	: void 		incrementSimulationTime() { simulationTime+=dt;};


	public	: void 		logMessage(const string& str);
	public	: void 		logError(const string& str);
	
	private	: void 		init();
	private	: Omega();
	private	: ~Omega();
	private	: Omega(const Omega&);
	private	: Omega& operator=(const Omega&);
	
	friend class Singleton< Omega >;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


#endif // __OMEGA_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
