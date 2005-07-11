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

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __ACTION_VEC_VEC_HPP__
#define __ACTION_VEC_VEC_HPP__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/PhysicalActionContainer.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <list>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class PhysicalAction;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace boost;
using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

// NEW CONTAINER version - faster

class PhysicalActionVectorVector : public PhysicalActionContainer
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Internal data										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
//	private : int currentIndex;
	
	// this in fact should be also a RedirectionVector in respect to the Body.id
	// this container is memory-consuming, because size of this vector is depending on highest id
	// from all bodies, not on the number of bodies
	
	// in this two-dimensional table:
	// 	- first  dimension is Body->getId() number
	//	- second dimension is PhysicalAction->getClassIndex() number
	private	  : vector< vector< shared_ptr<PhysicalAction> > > actionParameters;
	private	  :         vector< shared_ptr<PhysicalAction> >   actionTypesResetted;
	private	  : vector< bool > usedIds;
	
	//private	  : vector< vector< shared_ptr<PhysicalAction> > >::iterator vvi;
	//private	  : vector< vector< shared_ptr<PhysicalAction> > >::iterator vviEnd;
	//private	  :         vector< shared_ptr<PhysicalAction> >  ::iterator vi;
	//private	  :         vector< shared_ptr<PhysicalAction> >  ::iterator viEnd;
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////

	public	  : PhysicalActionVectorVector();
	public	  : virtual ~PhysicalActionVectorVector();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Methods											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	public    : virtual void clear();
	// doesn't not delete all, just resets data
	public    : virtual void reset();
	public    : virtual unsigned int size();
	// fills container with resetted fields. argument here, should be all PhysicalAction types that are planned to use
	public    : virtual void prepare(std::vector<shared_ptr<PhysicalAction> >& );
	
	// finds and returns action of given polymorphic type, for body of given Id,
	// should be always succesfull. if it is not - you forgot to call prepare()
	public    : virtual shared_ptr<PhysicalAction>& find(
					  unsigned int /*Body->getId() */
					, int /*ActionForce::getClassIndexStatic()*/);

	// looping over Bodies, and their Actions
	//public    : virtual void gotoFirst();
	//public    : virtual bool notAtEnd();
	//public    : virtual void gotoNext();
	//public    : virtual shared_ptr<PhysicalAction>& getCurrent(int& );

	public    : virtual PhysicalActionContainer::iterator begin();
        public    : virtual PhysicalActionContainer::iterator end();


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(PhysicalActionVectorVector);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(PhysicalActionVectorVector,false);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ACTION_VEC_VEC_HPP__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
