/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#include <yade/pkg-common/InteractingEdge.hpp>
#include "InteractingEdge2AABB.hpp"
#include <yade/pkg-common/AABB.hpp>

void InteractingEdge2AABB::go(	  const shared_ptr<InteractingGeometry>& cm
				, shared_ptr<BoundingVolume>& bv
				, const Se3r& se3
				, const Body*	)
{
	AABB* aabb = static_cast<AABB*>(bv.get());
	Vector3r edge = static_cast<InteractingEdge*>(cm.get())->edge;

	Vector3r p0 = se3.position;
	Vector3r p1 = se3.position + se3.orientation.Rotate(edge);
	
	aabb->min = componentMinVector(p0,p1);
	aabb->max = componentMaxVector(p0,p1);	
	
	aabb->halfSize = (aabb->max - aabb->min)/2;
	aabb->center = aabb->min + aabb->halfSize;
}
	
YADE_PLUGIN();
