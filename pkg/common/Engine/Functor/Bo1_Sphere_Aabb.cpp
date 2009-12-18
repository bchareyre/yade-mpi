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
		const Vector3r& sin(scene->cell._shearSin); const Vector3r& cos(scene->cell._shearCos); const Vector3r& tan(scene->cell.shear);
		Vector3r fac; //enlarge factors
		for(int i=0; i<3; i++) fac[i]=cos[i]*(1+pow(sin[i],2))/(1-pow(tan[i],2));
		for(int i=0; i<3; i++) halfSize[i]*=fac[(i+1)%3]*fac[(i+2)%3];
	}
	aabb->min = scene->cell.unshearPt(se3.position)-halfSize;
	aabb->max = scene->cell.unshearPt(se3.position)+halfSize;	
}
	
YADE_PLUGIN((Bo1_Sphere_Aabb));
