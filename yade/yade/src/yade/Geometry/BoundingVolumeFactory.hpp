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

#include "BoundingVolume.hpp"
#include "CollisionGeometry.hpp"
#include "Serializable.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief Abstract interface for all bounding volume factories.
	It is used for creating a bounding volume from a collision model during runtime.
	This is very useful when it is not trivial to build the bounding volume from the collision model. For example if you want to build an AABB from a box which is not initially aligned with the world axis, it is not easy to write by hand into the configuration file the center and size of this AABB. Instead you can use a BoundingVolumeFactory that will compute for you the correct value	
*/
class BoundingVolumeFactory : public Factorable
{	

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////

	/*! Constructor */
	public : BoundingVolumeFactory ();
	
	/*! Destructor */
	public : virtual ~BoundingVolumeFactory ();
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Methods											///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	/*! Method called to build a given bounding volume from a given collision model and a 3D transformation
		\param cm the collision model from wich we want to extract the bounding volume
		\param se3 the 3D transformation to apply to the collision model before building the bounding volume
		\return a shared pointer to the bounding volume
	*/
	public : virtual void buildBoundingVolume(const shared_ptr<CollisionGeometry> cm, const Se3r& se3,shared_ptr<BoundingVolume> bv) = 0;
	//FIXME : add also updateBoundingVolume(cm..). In fact we can load them automatically as we do with collisionFunctor because their name are Terrain2AABB ....
	
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __BOUNDINGVOLUMEFACTORY_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
