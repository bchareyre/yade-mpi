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

#ifndef __PHYSICALINTERACTOR_HPP__
#define __PHYSICALINTERACTOR_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Actor.hpp"
#include "DynLibDispatcher.hpp"
#include "BodyPhysicalParameters.hpp"
#include "Interaction.hpp"
#include "InteractionPhysicsFunctor.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class InteractionPhysicsDispatcher : public Actor
{
	protected: DynLibDispatcher
		<	TYPELIST_2( BodyPhysicalParameters , BodyPhysicalParameters ) ,	// base classess for dispatch
			InteractionPhysicsFunctor,					// class that provides multivirtual call
			void ,							// return type
			TYPELIST_3(
					  const shared_ptr<BodyPhysicalParameters>&	// arguments
					, const shared_ptr<BodyPhysicalParameters>&
					, shared_ptr<Interaction>&
				)
		> interactionPhysicsDispatcher;

	private : vector<vector<string> > interactionPhysicsFunctors;
	public  : void addInteractionPhysicsFunctor(const string& str1,const string& str2,const string& str3);
		
	// construction
	public : InteractionPhysicsDispatcher ();

	public : virtual ~InteractionPhysicsDispatcher ();


	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : virtual void action(Body* body);

	REGISTER_CLASS_NAME(InteractionPhysicsDispatcher);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(InteractionPhysicsDispatcher,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __PHYSICALINTERACTOR_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

