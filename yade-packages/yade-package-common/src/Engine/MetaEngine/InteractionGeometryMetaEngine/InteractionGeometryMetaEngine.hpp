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

#include <yade/yade-core/Interaction.hpp>
#include <yade/yade-lib-multimethods/DynLibDispatcher.hpp>
#include <yade/yade-core/MetaDispatchingEngine2D.hpp>
#include <yade/yade-core/InteractingGeometry.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InteractionGeometryEngineUnit.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Body;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class InteractionGeometryMetaEngine :	public MetaDispatchingEngine2D
					<	
						InteractingGeometry,						// base classe for dispatch
						InteractingGeometry,						// base classe for dispatch
						InteractionGeometryEngineUnit,					// class that provides multivirtual call
						bool ,								// return type
						TYPELIST_5(	  const shared_ptr<InteractingGeometry>&	// arguments
								, const shared_ptr<InteractingGeometry>&
								, const Se3r&
								, const Se3r&
								, const shared_ptr<Interaction>&
							  )
						, false								// disable auto symmetry handling
					>
{
	public    	: virtual void action(Body* body);

	REGISTER_ENGINE_UNIT_TYPE(InteractionGeometryEngineUnit);
	REGISTER_BASE_CLASS_TYPE_2D(InteractingGeometry,InteractingGeometry);
	REGISTER_CLASS_NAME(InteractionGeometryMetaEngine);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(InteractionGeometryMetaEngine,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __NARROWPHASECOLLIDER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
