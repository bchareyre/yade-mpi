/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#pragma once

#include<yade/pkg-common/BoundFunctor.hpp>

class Bo1_Sphere_Aabb : public BoundFunctor
{
	public :
		Bo1_Sphere_Aabb(): aabbEnlargeFactor(-1.) {}
		void go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r&, const Body*);
		double aabbEnlargeFactor;
	FUNCTOR2D(Sphere,Aabb);
	REGISTER_ATTRIBUTES(BoundFunctor,(aabbEnlargeFactor));
	REGISTER_CLASS_AND_BASE(Bo1_Sphere_Aabb,BoundFunctor);
};

REGISTER_SERIALIZABLE(Bo1_Sphere_Aabb);


