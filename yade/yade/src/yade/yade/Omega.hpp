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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Vector3.hpp"
#include <iostream>
#include <fstream>
#include <boost/shared_ptr.hpp>
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
	public : shared_ptr<FrontEnd> gui;

	public : shared_ptr<boost::thread> simulationThread;
	public : shared_ptr<SimulationLoop> simulationLoop; // FIXME put private and add function into omega to build them

	public : shared_ptr<ThreadSynchronizer> synchronizer; // FIXME put private and add function into omega to build them

	public : map<string,string> dynlibsType;

	public : bool getDynlibType(const string& libName,string& type);

	public : shared_ptr<ofstream> logFile;

// FIXME - this must be a pimpl, or removed somewhere else. (circular dependency loop : 4 hours lost to find it, janek)
	//private : float gravity_x,gravity_y,gravity_z;
	private : Vector3r gravity;
	public : Vector3r getGravity();
	public : void setGravity(Vector3r g);


	public : shared_ptr<NonConnexBody> rootBody;
	public : ptime startingSimulationTime;

	private : void buildDynlibList();
	private : void registerDynlibType(const string& name);

	public	: float 	dt;
	public	: void 		setTimestep(const string);
	public	: float 	getTimestep();


	private	: string 	fileName;
	public	: void 		setFileName(const string);
	public	: string 	getFileName();
	public	: void		loadTheFile();

	private	: long int 	maxiter;
	public  : long int	iter;
	public  : long int&	getIterReference();
	public  : long int	getIter();

	public	: void 		setMaxiter(const string);
	public	: long int 	getMaxiter();
	// FIXME - move this junk somewhere else...
	private : bool		automatic;
	public  : void		setAutomatic(bool);
	public  : bool		getAutomatic();
	// FIXME - move this junk somewhere else...
	private : bool		progress;
	public  : void		setProgress(bool);
	public  : bool		getProgress();




	public : void init();
	private   : Omega() ;
	private   : ~Omega() ;
	private   : Omega(const Omega&);
	private   : Omega& operator=(const Omega&);

	friend class Singleton< Omega >;

	public : void logMessage(const string& str);
	public : void logError(const string& str);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


#endif // __OMEGA_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
