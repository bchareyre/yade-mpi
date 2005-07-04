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
 
#ifndef __BOUNDINGVOLUMEUPDATOR_HPP__
#define __BOUNDINGVOLUMEUPDATOR_HPP__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/Engine.hpp>
#include <yade-lib-multimethods/DynLibDispatcher.hpp>
#include <yade/GeometricalModel.hpp>
#include <yade/InteractingGeometry.hpp>
#include <yade-common/InteractingGeometryEngineUnit.hpp>
#include <yade/Body.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class InteractingGeometryMetaEngine : 
	  public Engine 
	, public DynLibDispatcher
		<	TYPELIST_2( GeometricalModel , InteractingGeometry ) ,		// base classess for dispatch
			InteractingGeometryEngineUnit,						// class that provides multivirtual call
			void ,								// return type
			TYPELIST_4(
					  const shared_ptr<GeometricalModel>&	// arguments
					, shared_ptr<InteractingGeometry>& // is not working when const, because functors are supposed to modify it!
					, const Se3r&
					, const Body* // with that - functors have all the data they may need, but it's const, so they can't modify it !
					)
		>
{
	public		: virtual void action(Body* b);
	public		: virtual void registerAttributes();
	public		: virtual void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(InteractingGeometryMetaEngine);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(InteractingGeometryMetaEngine,false);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __BOUNDINGVOLUMEUPDATOR_HPP__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
