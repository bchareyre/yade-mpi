/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#include "Bo1_Sphere_Aabb.hpp"
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-common/Aabb.hpp>

void Bo1_Sphere_Aabb::go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body*){
	Sphere* sphere = static_cast<Sphere*>(cm.get());
	Aabb* aabb = static_cast<Aabb*>(bv.get());
	aabb->center = se3.position;
	aabb->halfSize = (aabbEnlargeFactor>0?aabbEnlargeFactor:1.)*Vector3r(sphere->radius,sphere->radius,sphere->radius);
	
	aabb->min = aabb->center-aabb->halfSize;
	aabb->max = aabb->center+aabb->halfSize;	
}
	
YADE_PLUGIN((Bo1_Sphere_Aabb));
