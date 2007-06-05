/*************************************************************************
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#include "InteractingSphere2AABBwater.hpp"
#include <yade/pkg-common/InteractingSphere.hpp>
#include <yade/pkg-common/AABB.hpp>

void InteractingSphere2AABBwater::go(	  const shared_ptr<InteractingGeometry>& cm
				, shared_ptr<BoundingVolume>& bv
				, const Se3r& se3
				, const Body*	)
{
	InteractingSphere* sphere = static_cast<InteractingSphere*>(cm.get());
	AABB* aabb = static_cast<AABB*>(bv.get());
	
	aabb->center = se3.position;
	
	aabb->halfSize = Vector3r(1.5*sphere->radius,1.5*sphere->radius,1.5*sphere->radius);
	
	aabb->min = aabb->center-aabb->halfSize;
	aabb->max = aabb->center+aabb->halfSize;	

}
	
