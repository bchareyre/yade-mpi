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
#include "InteractingGeometry.hpp"
#include "InteractionGeometryFunctor.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Body;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class InteractionGeometryDispatcher : 
	  public Actor
	, public DynLibDispatcher
		<	TYPELIST_2( InteractingGeometry , InteractingGeometry ) ,	// base classess for dispatch
			InteractionGeometryFunctor,					// class that provides multivirtual call
			bool ,								// return type
			TYPELIST_5(
					  const shared_ptr<InteractingGeometry>&	// arguments
					, const shared_ptr<InteractingGeometry>&
					, const Se3r&
					, const Se3r&
					, const shared_ptr<Interaction>&
				)
			, false								// disable auto symmetry handling
		>
{
	public    	: virtual void action(Body* body);
	public    	: virtual void registerAttributes();
	protected 	: virtual void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(InteractionGeometryDispatcher);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(InteractionGeometryDispatcher,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __NARROWPHASECOLLIDER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
