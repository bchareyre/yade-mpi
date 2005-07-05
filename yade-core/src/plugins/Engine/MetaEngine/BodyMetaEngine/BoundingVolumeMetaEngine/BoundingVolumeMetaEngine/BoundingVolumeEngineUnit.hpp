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

#ifndef __BOUNDINGVOLUMEFACTORY_H__
#define __BOUNDINGVOLUMEFACTORY_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-lib-multimethods/FunctorWrapper.hpp>
#include <yade/yade-core/BoundingVolume.hpp>
#include <yade/yade-core/InteractingGeometry.hpp>
#include <yade/yade-core/Body.hpp>
#include <yade/yade-core/MetaBody.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/shared_ptr.hpp>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief Abstract interface for all bounding volume factories.
	It is used for creating a bounding volume from a interaction geometry during runtime.
	This is very useful when it is not trivial to build the bounding volume from the interaction model.
	 
	For example if you want to build an AABB from a box which is not initially aligned with the world
	axis, it is not easy to write by hand into the configuration file the center and size of this AABB.
	Instead you can use a BoundingVolumeEngineUnit that will compute for you the correct value	
*/
class BoundingVolumeEngineUnit : public FunctorWrapper
	/*! Method called to build a given bounding volume from a given collision model and a 3D transformation
		\param const shared_ptr<InteractingGeometry>& the collision model from wich we want
		to extract the bounding volume
		\param Se3r& the 3D transformation to apply to the collision model before building
		the bounding volume
		\param Body* the Body inside which operation takes place
		\return shared_ptr<BoundingVolume>& shared pointer to the bounding volume
	*/
		<
		 void ,
		 TYPELIST_4(
		 		  const shared_ptr<InteractingGeometry>&
				, shared_ptr<BoundingVolume>& // is not working when const, because functors are supposed to modify it!
				, const Se3r& // FIXME - remove Se3r, because not everything is supposed to have it. If some function needs Se3r it must find it through Body*
				, const Body* // with that - functors have all the data they may need, but it's const, so they can't modify it !
			  )
		>
{	
	REGISTER_CLASS_NAME(BoundingVolumeEngineUnit);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(BoundingVolumeEngineUnit,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __BOUNDINGVOLUMEFACTORY_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
