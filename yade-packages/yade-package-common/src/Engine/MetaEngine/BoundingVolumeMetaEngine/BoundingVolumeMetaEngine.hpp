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

#include "BoundingVolumeEngineUnit.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/MetaDispatchingEngine2D.hpp>
#include <yade/yade-lib-multimethods/DynLibDispatcher.hpp>
#include <yade/yade-core/InteractingGeometry.hpp>
#include <yade/yade-core/BoundingVolume.hpp>
#include <yade/yade-core/Body.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class BoundingVolumeMetaEngine :	public MetaDispatchingEngine2D
					<	
						InteractingGeometry,						// base classe for dispatch
						BoundingVolume,							// base classe for dispatch
						BoundingVolumeEngineUnit,					// class that provides multivirtual call
						void,								// return type
						TYPELIST_4(	  const shared_ptr<InteractingGeometry>&	// arguments
								, shared_ptr<BoundingVolume>& 			// is not working when const, because functors are supposed to modify it!
								, const Se3r&
								, const Body* 					// with that - functors have all the data they may need, but it's const, so they can't modify it !
							  )
					>
{
	public		: virtual void action(Body* b);

	REGISTER_ENGINE_UNIT_TYPE(BoundingVolumeEngineUnit);
	REGISTER_BASE_CLASS_TYPE_2D(InteractingGeometry,BoundingVolume);
	REGISTER_CLASS_NAME(BoundingVolumeMetaEngine);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(BoundingVolumeMetaEngine,false);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __BOUNDINGVOLUMEUPDATOR_HPP__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
