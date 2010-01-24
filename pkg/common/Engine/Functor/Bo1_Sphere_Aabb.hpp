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
	YADE_CLASS_BASE_DOC_ATTRS(Bo1_Sphere_Aabb,BoundFunctor,"Functor creating Aabb from Sphere",
		((aabbEnlargeFactor,"Relative enlargement of the bounding box (e.g. for distant interaction detection); deactivated if negative, which is the default."))
	);
};

REGISTER_SERIALIZABLE(Bo1_Sphere_Aabb);


