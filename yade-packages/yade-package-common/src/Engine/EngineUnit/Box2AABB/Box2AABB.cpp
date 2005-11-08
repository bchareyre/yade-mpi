/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#include "Box2AABB.hpp"
#include "InteractingBox.hpp"
#include "AABB.hpp"


void Box2AABB::go(	const shared_ptr<InteractingGeometry>& cm,
				shared_ptr<BoundingVolume>& bv,
				const Se3r& se3,
				const Body*	)
{
	InteractingBox* box = static_cast<InteractingBox*>(cm.get());
	AABB* aabb = static_cast<AABB*>(bv.get());
	
	aabb->center = se3.position;

	Matrix3r r;
	se3.orientation.toRotationMatrix(r);
	aabb->halfSize = Vector3r(0,0,0);
	for( int i=0; i<3; ++i )
		for( int j=0; j<3; ++j )
			aabb->halfSize[i] += fabs( r[i][j] * box->extents[j] );
	
	aabb->min = aabb->center-aabb->halfSize;
	aabb->max = aabb->center+aabb->halfSize;
}
	
