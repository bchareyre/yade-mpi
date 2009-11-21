/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#pragma once

#include<yade/pkg-common/BoundingVolumeEngineUnit.hpp>

class InteractingSphere2AABB : public BoundingVolumeEngineUnit
{
	public :
		InteractingSphere2AABB(): aabbEnlargeFactor(-1.) {}
		void go(const shared_ptr<InteractingGeometry>& cm, shared_ptr<BoundingVolume>& bv, const Se3r&, const Body*);
		double aabbEnlargeFactor;
	FUNCTOR2D(InteractingSphere,AABB);
	REGISTER_ATTRIBUTES(BoundingVolumeEngineUnit,(aabbEnlargeFactor));
	REGISTER_CLASS_AND_BASE(InteractingSphere2AABB,BoundingVolumeEngineUnit);
};

REGISTER_SERIALIZABLE(InteractingSphere2AABB);


