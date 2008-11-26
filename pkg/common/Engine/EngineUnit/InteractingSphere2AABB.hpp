/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#ifndef SPHERE2AABB_HPP
#define SPHERE2AABB_HPP

#include<yade/pkg-common/BoundingVolumeEngineUnit.hpp>

class InteractingSphere2AABB : public BoundingVolumeEngineUnit
{
	public :
		InteractingSphere2AABB(): aabbEnlargeFactor(1.) {}
		void go(const shared_ptr<InteractingGeometry>& cm, shared_ptr<BoundingVolume>& bv, const Se3r& se3, const Body*);
		double aabbEnlargeFactor;
	FUNCTOR2D(InteractingSphere,AABB);
	virtual void registerAttributes(){REGISTER_ATTRIBUTE(aabbEnlargeFactor);}
	REGISTER_CLASS_NAME(InteractingSphere2AABB);
	REGISTER_BASE_CLASS_NAME(BoundingVolumeEngineUnit);
};

REGISTER_SERIALIZABLE(InteractingSphere2AABB);

#endif // SPHERE2AABB_HPP

