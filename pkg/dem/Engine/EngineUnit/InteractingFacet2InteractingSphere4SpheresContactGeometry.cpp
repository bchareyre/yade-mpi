/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"InteractingFacet2InteractingSphere4SpheresContactGeometry.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/InteractingFacet.hpp>

#include<yade/lib-base/yadeWm3Extra.hpp>


InteractingFacet2InteractingSphere4SpheresContactGeometry::InteractingFacet2InteractingSphere4SpheresContactGeometry()
{
}

void InteractingFacet2InteractingSphere4SpheresContactGeometry::registerAttributes()
{	
}

bool InteractingFacet2InteractingSphere4SpheresContactGeometry::go(	const shared_ptr<InteractingGeometry>& cm1,
							const shared_ptr<InteractingGeometry>& cm2,
							const Se3r& se31,
							const Se3r& se32,
							const shared_ptr<Interaction>& c)
{
	InteractingFacet*   facet = static_cast<InteractingFacet*>(cm1.get());
	
	Matrix3r facetAxisT; se31.orientation.ToRotationMatrix(facetAxisT); 
	Matrix3r facetAxis = facetAxisT.Transpose();
	
	// local orientation
	Vector3r sp = facetAxis*(se32.position - se31.position); 
	
	Vector3r normal = facet->normal;
	Real L = normal.Dot(sp);
	if (L<0) { normal=-normal; L=-L; }
	Real sphereRadius = static_cast<InteractingSphere*>(cm2.get())->radius;

	if (L < sphereRadius) 
	{
	    sp -= normal * L;

	    int N = facet->vertices.size();
	    if (facet->edgeNormals[0].Dot(sp) <= 0) return false;
	    if (facet->edgeNormals[N].Dot(sp) <= 0) return false;
	    for (int i=1; i<N; ++i)
		if (facet->edgeNormals[i].Dot(sp - facet->vertices[i]) <= 0) return false;

	    
  	    shared_ptr<SpheresContactGeometry> scm;
  	    if (c->interactionGeometry)
  		scm = YADE_PTR_CAST<SpheresContactGeometry>(c->interactionGeometry);
  	    else
  		scm = shared_ptr<SpheresContactGeometry>(new SpheresContactGeometry());
  
	    Real penetrationDepth = sphereRadius - L;

	    // global orientation
	    normal = facetAxisT*normal; normal.Normalize();

  	    scm->contactPoint = se32.position - 0.5*penetrationDepth*normal; 
  	    scm->normal = normal; 
  	    scm->penetrationDepth = penetrationDepth;
  	    scm->radius1 = 2*sphereRadius;
  	    scm->radius2 = sphereRadius;
  			    
  	    if (!c->interactionGeometry)
  		    c->interactionGeometry = scm;
      
  	    return true;
	}
	return false;
}


bool InteractingFacet2InteractingSphere4SpheresContactGeometry::goReverse(	const shared_ptr<InteractingGeometry>& cm1,
								const shared_ptr<InteractingGeometry>& cm2,
								const Se3r& se31,
								const Se3r& se32,
								const shared_ptr<Interaction>& c)
{
	bool isInteracting = go(cm2,cm1,se32,se31,c);
	if (isInteracting)
	{
	    SpheresContactGeometry* scm = static_cast<SpheresContactGeometry*>(c->interactionGeometry.get());
	    scm->normal = -scm->normal;
	    Real tmpR = scm->radius1;
	    scm->radius1 = scm->radius2;
	    scm->radius2 = tmpR;
	}
	return isInteracting;
}

YADE_PLUGIN();
