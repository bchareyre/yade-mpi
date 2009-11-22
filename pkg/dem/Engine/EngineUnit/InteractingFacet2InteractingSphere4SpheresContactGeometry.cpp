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

CREATE_LOGGER(InteractingFacet2InteractingSphere4SpheresContactGeometry);

InteractingFacet2InteractingSphere4SpheresContactGeometry::InteractingFacet2InteractingSphere4SpheresContactGeometry() 
{
	shrinkFactor=0;
}

bool InteractingFacet2InteractingSphere4SpheresContactGeometry::go(const shared_ptr<InteractingGeometry>& cm1,
							const shared_ptr<InteractingGeometry>& cm2,
							const Se3r& se31,
							const Se3r& se32,
							const shared_ptr<Interaction>& c)
{
	InteractingFacet*   facet = static_cast<InteractingFacet*>(cm1.get());
	/* could be written as (needs to be tested):
	 * Vector3r cl=se31.orientation.Conjugate()*(se32.position-se31.position);
	 */
	Matrix3r facetAxisT; se31.orientation.ToRotationMatrix(facetAxisT); 
	Matrix3r facetAxis = facetAxisT.Transpose();
	// local orientation
	Vector3r cl = facetAxis*(se32.position - se31.position);  // "contact line" in facet-local coords

	//
	// BEGIN everything in facet-local coordinates
	//

	Vector3r normal = facet->nf;
	Real L = normal.Dot(cl);
	if (L<0) {normal=-normal; L=-L; }

	Real sphereRadius = static_cast<InteractingSphere*>(cm2.get())->radius;
	if (L>sphereRadius && !c->isReal())  return false; // no contact, but only if there was no previous contact; ortherwise, the constitutive law is responsible for setting Interaction::isReal=false

	Vector3r cp = cl - L*normal;
	const Vector3r* ne = facet->ne;
	
	Real penetrationDepth=0;
	
	Real bm = ne[0].Dot(cp);
	int m=0;
	for (int i=1; i<3; ++i)
	{
		Real b=ne[i].Dot(cp);
		if (bm<b) {bm=b; m=i;}
	}

	Real sh = sphereRadius*shrinkFactor;
	Real icr = facet->icr-sh;

	if (icr<0)
	{
		LOG_WARN("a radius of a facet's inscribed circle less than zero! So, shrinkFactor is too large and would be reduced to zero.");
		shrinkFactor=0;
		icr = facet->icr;
		sh = 0;
	}


	if (bm<icr) // contact with facet's surface
	{
		penetrationDepth = sphereRadius - L;
		normal.Normalize();
	}
	else
	{
		cp = cp + ne[m]*(icr-bm);
		if (cp.Dot(ne[(m-1<0)?2:m-1])>icr) // contact with vertex m
//			cp = facet->vertices[m];
			cp = facet->vu[m]*(facet->vl[m]-sh);
		else if (cp.Dot(ne[m=(m+1>2)?0:m+1])>icr) // contact with vertex m+1
//			cp = facet->vertices[(m+1>2)?0:m+1];
			cp = facet->vu[m]*(facet->vl[m]-sh);
		normal = cl-cp;
		penetrationDepth = sphereRadius - normal.Normalize();
	}

	//
	// END everything in facet-local coordinates
	//

	if (penetrationDepth>0 || c->isReal())
	{
		shared_ptr<SpheresContactGeometry> scm;
		if (c->interactionGeometry)
			scm = YADE_PTR_CAST<SpheresContactGeometry>(c->interactionGeometry);
		else
			scm = shared_ptr<SpheresContactGeometry>(new SpheresContactGeometry());
	  
		normal = facetAxisT*normal; // in global orientation
		scm->contactPoint = se32.position - (sphereRadius-0.5*penetrationDepth)*normal; 
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
	c->swapOrder();
	//LOG_WARN("Swapped interaction order for "<<c->getId2()<<"&"<<c->getId1());
	return go(cm2,cm1,se32,se31,c);
}

YADE_PLUGIN((InteractingFacet2InteractingSphere4SpheresContactGeometry));

//YADE_REQUIRE_FEATURE(PHYSPAR);

