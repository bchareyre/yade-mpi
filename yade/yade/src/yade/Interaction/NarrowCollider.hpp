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

#ifndef __NARROWPHASECOLLIDER_H__
#define __NARROWPHASECOLLIDER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <list>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Interaction.hpp"
#include "DynLibDispatcher.hpp"
#include "Actor.hpp"
#include "CollisionGeometry.hpp"
#include "CollisionFunctor.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Body;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class NarrowCollider : public Actor
{
	protected: DynLibDispatcher
		<	TYPELIST_2( CollisionGeometry , CollisionGeometry ) ,	// base classess for dispatch
			CollisionFunctor,					// class that provides multivirtual call
			bool ,							// return type
			TYPELIST_5(
					const shared_ptr<CollisionGeometry>&	// arguments
					, const shared_ptr<CollisionGeometry>&
					, const Se3r&
					, const Se3r&
					, shared_ptr<Interaction>&
				)
			, false
		> narrowCollisionDispatcher;

	private : vector<vector<string> > collisionFunctors;
	public  : void addCollisionFunctor(const string& str1,const string& str2,const string& str3);
		
	// construction
	public : NarrowCollider ();

	public : virtual ~NarrowCollider ();

	public : virtual void narrowCollisionPhase(Body* ) { throw;};

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	//public : virtual bool isActivated();
	public : virtual void action(Body* b);

	REGISTER_CLASS_NAME(NarrowCollider);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(NarrowCollider,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __NARROWPHASECOLLIDER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
