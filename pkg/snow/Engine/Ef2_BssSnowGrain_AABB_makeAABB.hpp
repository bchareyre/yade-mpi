/*************************************************************************
*  Copyright (C) 2009 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#pragma once

#include<yade/pkg-common/BoundingVolumeFunctor.hpp>

class Ef2_BssSnowGrain_AABB_makeAABB : public BoundingVolumeFunctor
{
	public :
		Ef2_BssSnowGrain_AABB_makeAABB() {};
		void go(const shared_ptr<InteractingGeometry>& cm, shared_ptr<BoundingVolume>& bv, const Se3r& se3, const Body*);

	FUNCTOR2D(BssSnowGrain,AABB);
	REGISTER_CLASS_NAME(Ef2_BssSnowGrain_AABB_makeAABB);
	REGISTER_BASE_CLASS_NAME(BoundingVolumeFunctor);
};

REGISTER_SERIALIZABLE(Ef2_BssSnowGrain_AABB_makeAABB);

