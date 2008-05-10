/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"InteractingVertex2InteractingSphere4SpheresContactGeometry.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/InteractingVertex.hpp>

#include<yade/lib-base/yadeWm3Extra.hpp>


InteractingVertex2InteractingSphere4SpheresContactGeometry::InteractingVertex2InteractingSphere4SpheresContactGeometry()
{
}

void InteractingVertex2InteractingSphere4SpheresContactGeometry::registerAttributes()
{	
}

bool InteractingVertex2InteractingSphere4SpheresContactGeometry::go(	const shared_ptr<InteractingGeometry>& cm1,
							const shared_ptr<InteractingGeometry>& cm2,
							const Se3r& se31,
							const Se3r& se32,
							const shared_ptr<Interaction>& c)
{

    InteractingSphere* s2 = static_cast<InteractingSphere*>(cm2.get());

    Vector3r normal = se32.position-se31.position; // TODO: Orientation!!!

    InteractingVertex* v = static_cast<InteractingVertex*>(cm1.get());

    // ignore vertices of a flat
    if (v->flat) return false; 

    for(int i=0,ei=v->normals.size(); i<ei; ++i)
	if ( normal.Dot(v->normals[i]) < 0 ) return false;

    Real penetrationDepth = s2->radius - normal.Normalize();
	    
    if (penetrationDepth>0)
    {

	    shared_ptr<SpheresContactGeometry> scm;
	    if (c->interactionGeometry)
		scm = YADE_PTR_CAST<SpheresContactGeometry>(c->interactionGeometry);
	    else
		scm = shared_ptr<SpheresContactGeometry>(new SpheresContactGeometry());

	    scm->contactPoint = se31.position-0.5*penetrationDepth*normal;
	    scm->normal = normal;
	    scm->penetrationDepth = penetrationDepth;
	    scm->radius1 = 2*s2->radius;
	    scm->radius2 = s2->radius;
			    
	    if (!c->interactionGeometry)
		    c->interactionGeometry = scm;
    
	    return true;
    }
    else return false;
}


bool InteractingVertex2InteractingSphere4SpheresContactGeometry::goReverse(	const shared_ptr<InteractingGeometry>& cm1,
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
