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
	const Vector3r& O = se3.position;
	Matrix3r facetAxisT; se3.orientation.ToRotationMatrix(facetAxisT);
	const vector<Vector3r>& vertices=facet->vertices;
	aabb->min=aabb->max = O + facetAxisT * vertices[0];
	for (int i=1;i<3;++i)
	{
	    Vector3r v = O + facetAxisT * vertices[i];
	    aabb->min = componentMinVector( aabb->min, v);
	    aabb->max = componentMaxVector( aabb->max, v);
	}
	aabb->halfSize = (aabb->max - aabb->min)/2;
	aabb->center = aabb->min + aabb->halfSize;
}
	
YADE_PLUGIN((InteractingFacet2AABB));