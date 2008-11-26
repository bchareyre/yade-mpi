/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#ifndef EF2_BSS_SWEPT_SPHERE_LINE_SEGMENT__AABB__MAKE_AABB
#define EF2_BSS_SWEPT_SPHERE_LINE_SEGMENT__AABB__MAKE_AABB


#include<yade/pkg-common/BoundingVolumeEngineUnit.hpp>


class ef2_BssSweptSphereLineSegment_AABB_makeAABB : public BoundingVolumeEngineUnit
{
	public :
		void go(	  const shared_ptr<InteractingGeometry>& cm
				, shared_ptr<BoundingVolume>& bv
				, const Se3r& se3
				, const Body*	);
                FUNCTOR2D(BssSweptSphereLineSegment,AABB);
                REGISTER_CLASS_NAME(ef2_BssSweptSphereLineSegment_AABB_makeAABB);
	        REGISTER_BASE_CLASS_NAME(BoundingVolumeEngineUnit);
};

REGISTER_SERIALIZABLE(ef2_BssSweptSphereLineSegment_AABB_makeAABB);

#endif // EF2_BSS_SWEPT_SPHERE_LINE_SEGMENT__AABB__MAKE_AABB

