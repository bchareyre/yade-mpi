/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#include "MetaInteractingGeometry2AABB.hpp"


#include<yade/pkg-common/MetaInteractingGeometry.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>
#include<limits>


void MetaInteractingGeometry2AABB::go(	  const shared_ptr<InteractingGeometry>&
						, shared_ptr<BoundingVolume>& bv
						, const Se3r&
						, const Body* body )
{
	AABB* aabb = static_cast<AABB*>(bv.get());
	const Real& inf=std::numeric_limits<Real>::infinity();
	Vector3r mx(-inf,-inf,-inf);
	Vector3r mn(inf,inf,inf);
	
	const MetaBody * ncb = YADE_CAST<const MetaBody*>(body);
	
	FOREACH(const shared_ptr<Body>& b, *ncb->bodies){
		if(!b) continue;
		if(b->boundingVolume){
			for(int i=0; i<3; i++){
				if(!isinf(b->boundingVolume->max[i])) mx[i]=max(mx[i],b->boundingVolume->max[i]);
				if(!isinf(b->boundingVolume->min[i])) mn[i]=min(mn[i],b->boundingVolume->min[i]);
			}
		} else {
	 		mx=componentMaxVector(mx,b->physicalParameters->se3.position);
 			mn=componentMinVector(mn,b->physicalParameters->se3.position);
		}
	}
	
	aabb->center = (mx+mn)*0.5;
	aabb->halfSize = (mx-mn)*0.5;

	aabb->min = mn;
	aabb->max = mx;
}
	
YADE_PLUGIN((MetaInteractingGeometry2AABB));
