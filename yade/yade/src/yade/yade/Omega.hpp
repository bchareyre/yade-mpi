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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//#include "Vector3.hpp"
//#include "Chrono.hpp"
#include <iostream>
#include "Types.hpp"
#include "Singleton.hpp"
//#include "NonConnexBody.hpp"
//#include "CollisionFunctor.hpp"
//class CollisionFunctor;
class Vector3; // FIXME - this is acceptable, but should be removed.
//class Chrono;
class NonConnexBody;
//#include "MultiMethodsManager.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace boost::posix_time;
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Omega : public Singleton<Omega>
{
	public : shared_ptr<ofstream> logFile;

// FIXME - this must be a pimpl, or removed somewhere else. (circular dependency loop : 4 hours lost to find it, janek)
	private : float gravity_x,gravity_y,gravity_z;
	public : Vector3 getGravity();
	public : void setGravity(Vector3 g);


	public : shared_ptr<NonConnexBody> rootBody;
	public : ptime startingSimulationTime;

	public	: float 	dt;
	public	: void 		setTimestep(const string);
	public	: float 	getTimestep();

// FIXME - where to put settings? in another singleton, or in a class owned by Omega ?
	private	: string 	filename;
	public	: void 		setFilename(const string);
	public	: string 	getFilename();

	private	: long int 	maxiter;
	public	: void 		setMaxiter(const string);
	public	: long int 	getMaxiter();

	private : bool		automatic;
	public  : void		setAutomatic(bool);
	public  : bool		getAutomatic();

	//public : MultiMethodsManager<CollisionFunctor> narrowCollider;
	//public : MultiMethodsManager narrowCollider;

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
