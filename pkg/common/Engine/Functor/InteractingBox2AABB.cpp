/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#include "InteractingBox2AABB.hpp"
#include<yade/pkg-common/InteractingBox.hpp>
#include<yade/pkg-common/AABB.hpp>


void InteractingBox2AABB::go(	const shared_ptr<Shape>& cm,
				shared_ptr<Bound>& bv,
				const Se3r& se3,
				const Body*	)
{
	InteractingBox* box = static_cast<InteractingBox*>(cm.get());
	AABB* aabb = static_cast<AABB*>(bv.get());
	
	aabb->center = se3.position;

	Matrix3r r;
	se3.orientation.ToRotationMatrix(r);
	aabb->halfSize = Vector3r(0,0,0);
	for( int i=0; i<3; ++i )
		for( int j=0; j<3; ++j )
			aabb->halfSize[i] += fabs( r[i][j] * box->extents[j] );
	
	aabb->min = aabb->center-aabb->halfSize;
	aabb->max = aabb->center+aabb->halfSize;
}
	
YADE_PLUGIN((InteractingBox2AABB));
