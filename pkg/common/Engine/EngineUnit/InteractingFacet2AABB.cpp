/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#include <yade/pkg-common/InteractingFacet.hpp>
#include "InteractingFacet2AABB.hpp"
#include <yade/pkg-common/AABB.hpp>

void InteractingFacet2AABB::go(	  const shared_ptr<InteractingGeometry>& cm
				, shared_ptr<BoundingVolume>& bv
				, const Se3r& se3
				, const Body*	)
{
	AABB* aabb = static_cast<AABB*>(bv.get());
	InteractingFacet* facet = static_cast<InteractingFacet*>(cm.get());
	Vector3r v0 = se3.position;
	Matrix3r facetAxisT; se3.orientation.ToRotationMatrix(facetAxisT);

	aabb->min=aabb->max = v0;
	for (int i=0,e=facet->vertices.size();i<e;++i)
	{
	    Vector3r v = v0 + facetAxisT * facet->vertices[i];
	    aabb->min = componentMinVector( aabb->min, v);
	    aabb->max = componentMaxVector( aabb->max, v);
	}
	
	aabb->halfSize = (aabb->max - aabb->min)/2;
	aabb->center = aabb->min + aabb->halfSize;
}
	
YADE_PLUGIN();
