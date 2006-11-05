/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#include "InteractingMyTetrahedron2AABB.hpp"
#include "InteractingMyTetrahedron.hpp"
#include <yade/yade-package-common/AABB.hpp>

void InteractingMyTetrahedron2AABB::go(	  const shared_ptr<InteractingGeometry>& cm
				, shared_ptr<BoundingVolume>& bv
				, const Se3r& se3
				, const Body*	)
{
	InteractingMyTetrahedron* myTet = static_cast<InteractingMyTetrahedron*>(cm.get());
	AABB* aabb = static_cast<AABB*>(bv.get());

	// create a bounding volume from four spheres ( InteractingMyTetrahedron )
	Vector3r min = Vector3r(0,0,0);
	min = min.minVector(se3.orientation*myTet->c1 - Vector3r(myTet->r1,myTet->r1,myTet->r1));
	min = min.minVector(se3.orientation*myTet->c2 - Vector3r(myTet->r2,myTet->r2,myTet->r2));
	min = min.minVector(se3.orientation*myTet->c3 - Vector3r(myTet->r3,myTet->r3,myTet->r3));
	min = min.minVector(se3.orientation*myTet->c4 - Vector3r(myTet->r4,myTet->r4,myTet->r4));
	min += se3.position;

	Vector3r max = Vector3r(0,0,0);
	max = max.maxVector(se3.orientation*myTet->c1 + Vector3r(myTet->r1,myTet->r1,myTet->r1));
	max = max.maxVector(se3.orientation*myTet->c2 + Vector3r(myTet->r2,myTet->r2,myTet->r2));
	max = max.maxVector(se3.orientation*myTet->c3 + Vector3r(myTet->r3,myTet->r3,myTet->r3));
	max = max.maxVector(se3.orientation*myTet->c4 + Vector3r(myTet->r4,myTet->r4,myTet->r4));
	max += se3.position;

	// AABB
	aabb->center = (min+max)*0.5;
	aabb->halfSize = (max-min)*0.5;
	
	// BoundingVolume
	aabb->min = min;
	aabb->max = max;	
}
	
