/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"InteractingEdge2InteractingSphere4SpheresContactGeometry.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/InteractingEdge.hpp>

#include<yade/lib-base/yadeWm3Extra.hpp>


InteractingEdge2InteractingSphere4SpheresContactGeometry::InteractingEdge2InteractingSphere4SpheresContactGeometry()
{
}

void InteractingEdge2InteractingSphere4SpheresContactGeometry::registerAttributes()
{	
}

bool InteractingEdge2InteractingSphere4SpheresContactGeometry::go(	const shared_ptr<InteractingGeometry>& cm1,
							const shared_ptr<InteractingGeometry>& cm2,
							const Se3r& se31,
							const Se3r& se32,
							const shared_ptr<Interaction>& c)
{
	InteractingEdge*   e = static_cast<InteractingEdge*>(cm1.get());

	// FIXME: ignore edges on a flat?
	if (e->flat) return false; 

	Real squaredLength = e->squaredLength;
	Real sphereRadius = static_cast<InteractingSphere*>(cm2.get())->radius;

	Vector3r ep = se31.position;
	Vector3r sp = se32.position;
	Vector3r rp = sp - ep;
	
	// TODO: orientation normal1 & normal2!!!

	if (e->both) {
	    if ( e->normal1.Dot(rp) < 0 || e->normal2.Dot(rp) < 0 ) return false;
	} else if ( e->normal1.Dot(rp) < 0 ) return false;

	Vector3r edge = e->edge; // TODO: orientation edge!!! se31.orientation.Rotate(e->edge); 

	Real c1 = rp.Dot(edge);
	if ( c1<=0 || squaredLength<=c1 ) return false;

	Vector3r Pb = ep + c1/squaredLength*edge;

	Vector3r normal = sp - Pb;
	Real penetrationDepth = sphereRadius - normal.Normalize();

	if (penetrationDepth > 0)
	{
	    shared_ptr<SpheresContactGeometry> scm;
	    if (c->interactionGeometry)
		scm = YADE_PTR_CAST<SpheresContactGeometry>(c->interactionGeometry);
	    else
		scm = shared_ptr<SpheresContactGeometry>(new SpheresContactGeometry());

	    scm->contactPoint = Pb-0.5*penetrationDepth*normal;
	    scm->normal = normal;
	    scm->penetrationDepth = penetrationDepth;
	    scm->radius1 = 2*sphereRadius;
	    scm->radius2 = sphereRadius;
			    
	    if (!c->interactionGeometry)
		    c->interactionGeometry = scm;
    
	    return true;
	}
	else return false;
}


bool InteractingEdge2InteractingSphere4SpheresContactGeometry::goReverse(	const shared_ptr<InteractingGeometry>& cm1,
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
