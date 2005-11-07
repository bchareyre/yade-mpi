/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef BOUNDINGVOLUME_HPP
#define BOUNDINGVOLUME_HPP

#include <GL/glut.h>
#include <GL/gl.h>

#include <yade/yade-lib-wm3-math/Se3.hpp>
#include <yade/yade-lib-serialization/Serializable.hpp>
#include <yade/yade-lib-multimethods/Indexable.hpp>

/*! \brief Abstract interface for all bounding volumes.

	All the bounding volumes (BoundingSphere, AABB ...) derive from this class. A bounding volume is used to speed up the
	collision detection. Instead of computing if 2 complex polyhedrons collide each other, it is much faster to first test
	if their bounding volumes (for example a AABB) are in collision.
*/

class BoundingVolume : public Serializable, public Indexable
{

/// Attributes
	public : // FIXME - maybe private?
		Vector3r	 diffuseColor		/// Color of the bounding volume. Used only for drawing purpose
				,min			/// Minimum of the bounding volume
				,max;			/// Maximum of the bounding volume

/// Serialization
		void registerAttributes();
	REGISTER_CLASS_NAME(BoundingVolume);
	REGISTER_BASE_CLASS_NAME(Serializable Indexable);

	
/// Indexable
	REGISTER_INDEX_COUNTER(BoundingVolume);

};

REGISTER_SERIALIZABLE(BoundingVolume,false);

#endif // __BOUNDINGVOLUME_H__

