/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#include "Bo1_Box_Aabb.hpp"
#include<yade/pkg-common/Box.hpp>
#include<yade/pkg-common/Aabb.hpp>


void Bo1_Box_Aabb::go(	const shared_ptr<Shape>& cm,
				shared_ptr<Bound>& bv,
				const Se3r& se3,
				const Body*	)
{
	Box* box = static_cast<Box*>(cm.get());
	Aabb* aabb = static_cast<Aabb*>(bv.get());

	if(scene->isPeriodic && scene->cell->hasShear()) throw logic_error(__FILE__ "Boxes not (yet?) supported in sheared cell.");
	
	Matrix3r r;
	se3.orientation.ToRotationMatrix(r);
	Vector3r halfSize(Vector3r::ZERO);
	for( int i=0; i<3; ++i )
		for( int j=0; j<3; ++j )
			halfSize[i] += fabs( r[i][j] * box->extents[j] );
	
	aabb->min = se3.position-halfSize;
	aabb->max = se3.position+halfSize;
}
	
YADE_PLUGIN((Bo1_Box_Aabb));
