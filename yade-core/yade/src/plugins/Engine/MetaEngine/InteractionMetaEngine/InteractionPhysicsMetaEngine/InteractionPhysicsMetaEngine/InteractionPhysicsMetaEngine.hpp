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

#include <yade/Engine.hpp>
#include <yade-lib-multimethods/DynLibDispatcher.hpp>
#include <yade/PhysicalParameters.hpp>
#include <yade/Interaction.hpp>
#include "InteractionPhysicsEngineUnit.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class InteractionPhysicsMetaEngine : 
	  public Engine
	, public DynLibDispatcher
		<	TYPELIST_2( PhysicalParameters , PhysicalParameters ) ,	// base classess for dispatch
			InteractionPhysicsEngineUnit,					// class that provides multivirtual call
			void ,								// return type
			TYPELIST_3(	  const shared_ptr<PhysicalParameters>&	// arguments
					, const shared_ptr<PhysicalParameters>&
					, const shared_ptr<Interaction>&
				)
		>
{		
	public 		: virtual void action(Body* body);
	public 		: virtual void registerAttributes();
	protected 	: virtual void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(InteractionPhysicsMetaEngine);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(InteractionPhysicsMetaEngine,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __PHYSICALINTERACTOR_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

