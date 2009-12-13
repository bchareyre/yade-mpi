/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#pragma once

#include<yade/pkg-common/BoundFunctor.hpp>

class InteractingSphere2AABB : public BoundFunctor
{
	public :
		InteractingSphere2AABB(): aabbEnlargeFactor(-1.) {}
		void go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r&, const Body*);
		double aabbEnlargeFactor;
	FUNCTOR2D(Sphere,AABB);
	REGISTER_ATTRIBUTES(BoundFunctor,(aabbEnlargeFactor));
	REGISTER_CLASS_AND_BASE(InteractingSphere2AABB,BoundFunctor);
};

REGISTER_SERIALIZABLE(InteractingSphere2AABB);


