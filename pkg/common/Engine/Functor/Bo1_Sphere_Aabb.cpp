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

void Bo1_Sphere_Aabb::go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body* b){
	Sphere* sphere = static_cast<Sphere*>(cm.get());
	Aabb* aabb = static_cast<Aabb*>(bv.get());
	Vector3r halfSize = (aabbEnlargeFactor>0?aabbEnlargeFactor:1.)*Vector3r(sphere->radius,sphere->radius,sphere->radius);
	// adjust box size along axes so that sphere doesn't stick out of the box even if sheared (i.e. parallelepiped)
	if(scene->isPeriodic) {
		const Matrix3r& cos=scene->cell->getCosMatrix();
		for(int i=0; i<3; i++) halfSize[i]*=(1/cos[i][(i+1)%3])*(1/cos[i][(i+2)%3]);
	}
	aabb->min = scene->cell->unshearPt(se3.position)-halfSize;
	aabb->max = scene->cell->unshearPt(se3.position)+halfSize;	
}
	
YADE_PLUGIN((Bo1_Sphere_Aabb));
