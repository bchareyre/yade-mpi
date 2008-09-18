/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"InteractingSphere2InteractingSphere4SpheresContactGeometry.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>

#include<yade/lib-base/yadeWm3Extra.hpp>
#include<yade/core/Omega.hpp>


InteractingSphere2InteractingSphere4SpheresContactGeometry::InteractingSphere2InteractingSphere4SpheresContactGeometry()
{
	interactionDetectionFactor = 1;
}

void InteractingSphere2InteractingSphere4SpheresContactGeometry::registerAttributes()
{	
	REGISTER_ATTRIBUTE(interactionDetectionFactor);
	REGISTER_ATTRIBUTE(hasShear);
}

bool InteractingSphere2InteractingSphere4SpheresContactGeometry::go(	const shared_ptr<InteractingGeometry>& cm1,
							const shared_ptr<InteractingGeometry>& cm2,
							const Se3r& se31,
							const Se3r& se32,
							const shared_ptr<Interaction>& c)
{
	InteractingSphere* s1 = static_cast<InteractingSphere*>(cm1.get());
	InteractingSphere* s2 = static_cast<InteractingSphere*>(cm2.get());

	Vector3r normal = se32.position-se31.position;
	Real penetrationDepth = pow(interactionDetectionFactor*(s1->radius+s2->radius),2) - normal.SquaredLength();// Compute a wrong value just to check sign - faster than computing distances
	//Real penetrationDepth = s1->radius+s2->radius-normal.Normalize();
	if (penetrationDepth>0 || c->isReal){
		shared_ptr<SpheresContactGeometry> scm;
		if (c->interactionGeometry) scm = YADE_PTR_CAST<SpheresContactGeometry>(c->interactionGeometry);
		else scm = shared_ptr<SpheresContactGeometry>(new SpheresContactGeometry());

		penetrationDepth = s1->radius+s2->radius-normal.Normalize();
		scm->contactPoint = se31.position+(s1->radius-0.5*penetrationDepth)*normal;//0.5*(pt1+pt2);
		scm->normal = normal;
		scm->penetrationDepth = penetrationDepth;
		scm->radius1 = s1->radius;
		scm->radius2 = s2->radius;
		if (!c->interactionGeometry) c->interactionGeometry = scm;
		if(hasShear){
			scm->hasShear=true;
			scm->pos1=se31.position; scm->pos2=se32.position;
			scm->ori1=se31.orientation; scm->ori2=se32.orientation;
			if(c->isNew){
				// contact constants
				scm->d0=(se32.position-se31.position).Length();
				scm->d1=s1->radius-penetrationDepth; scm->d2=s2->radius-penetrationDepth;
				// quasi-constants
				scm->cp1rel.Align(Vector3r::UNIT_X,se31.orientation.Conjugate()*normal);
				scm->cp2rel.Align(Vector3r::UNIT_X,se32.orientation.Conjugate()*(-normal));
				scm->cp1rel.Normalize(); scm->cp2rel.Normalize();
			}
			// testing only
			#if 0
			if((Omega::instance().getCurrentIteration()%10000)==0) scm->relocateContactPoints();
			if((Omega::instance().getCurrentIteration()%10000)==0) {
				Real slip=scm->slipToEpsNMax(1.); if(slip>0) cerr<<"SLIP by Δε_N "<<slip<<" → ε_N="<<scm->epsN()<<endl;
			}
			#endif
		}
		return true;
	} else return false;
}


bool InteractingSphere2InteractingSphere4SpheresContactGeometry::goReverse(	const shared_ptr<InteractingGeometry>& cm1,
								const shared_ptr<InteractingGeometry>& cm2,
								const Se3r& se31,
								const Se3r& se32,
								const shared_ptr<Interaction>& c)
{
	return go(cm1,cm2,se31,se32,c);
}

YADE_PLUGIN();
