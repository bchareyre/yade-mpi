/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#include "InteractingSphere2AABB.hpp"
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/AABB.hpp>

void InteractingSphere2AABB::go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body*){
	InteractingSphere* sphere = static_cast<InteractingSphere*>(cm.get());
	AABB* aabb = static_cast<AABB*>(bv.get());
	aabb->center = se3.position;
	aabb->halfSize = (aabbEnlargeFactor>0?aabbEnlargeFactor:1.)*Vector3r(sphere->radius,sphere->radius,sphere->radius);
	
	aabb->min = aabb->center-aabb->halfSize;
	aabb->max = aabb->center+aabb->halfSize;	
}
	
YADE_PLUGIN((InteractingSphere2AABB));
