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


InteractingSphere2InteractingSphere4SpheresContactGeometry::InteractingSphere2InteractingSphere4SpheresContactGeometry()
{
	interactionDetectionFactor = 1;
}

void InteractingSphere2InteractingSphere4SpheresContactGeometry::registerAttributes()
{	
	REGISTER_ATTRIBUTE(interactionDetectionFactor);
	REGISTER_ATTRIBUTE(exactRot);
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
	Real penetrationDepth = pow(interactionDetectionFactor*(s1->radius+s2->radius), 2) - normal.SquaredLength();// Compute a wrong value just to check sign - faster than computing distances
	//Real penetrationDepth = s1->radius+s2->radius-normal.Normalize();
	if (penetrationDepth>0 || c->isReal){
		shared_ptr<SpheresContactGeometry> scm;
		if (c->interactionGeometry){
			// WARNING! 
			// FIXME - this must be dynamic cast until the contaners are rewritten to support multiple interactions types
			//         the problem is that scm can be either SDECLinkGeometry or SpheresContactGeometry and the only way CURRENTLY
			//         to check this is by dynamic cast. This has to be fixed.
			scm = YADE_PTR_CAST<SpheresContactGeometry>(c->interactionGeometry);
		} else scm = shared_ptr<SpheresContactGeometry>(new SpheresContactGeometry());
		penetrationDepth = s1->radius+s2->radius-normal.Normalize();
		scm->contactPoint = se31.position+(s1->radius-0.5*penetrationDepth)*normal;//0.5*(pt1+pt2);
		scm->normal = normal;
		scm->penetrationDepth = penetrationDepth;
		scm->radius1 = s1->radius;
		scm->radius2 = s2->radius;
		if (!c->interactionGeometry) c->interactionGeometry = scm;
		if(exactRot){
			scm->exactRot=true;
			scm->pos1=se31.position; scm->pos2=se32.position;
			scm->ori1=se31.orientation; scm->ori2=se32.orientation;
			//scm->ori1.Normalize(); scm->ori2.Normalize();
			if(c->isNew){
				//cerr<<"+++ Assigning constants to SpheresContactGeometry"<<endl;
				// contact constants
				scm->d0=(se32.position-se31.position).Length();
				scm->d1=s1->radius-penetrationDepth; scm->d2=s2->radius-penetrationDepth;
				// quasi-constants
				scm->cp1rel.Align(Vector3r::UNIT_X,se31.orientation.Conjugate()*normal);
				scm->cp2rel.Align(Vector3r::UNIT_X,se32.orientation.Conjugate()*(-normal));
				scm->cp1rel.Normalize(); scm->cp2rel.Normalize();
				cerr<<"+++ Relative orientations: "<<scm->cp1rel<<" | "<<scm->cp2rel<<endl;
				//cerr<<"+++ "<<se31.orientation.Conjugate()<<" | "<<se31.orientation.Conjugate()*normal<<"|"<<scm->cp1rel<<endl;
				//cerr<<"@@@ cp1rel="<<scm->cp1rel[0]<<";"<<scm->cp1rel[1]<<";"<<scm->cp1rel[2]<<";"<<scm->cp1rel[3]<<endl;
				//cerr<<"@@@ ori1="<<scm->ori1[0]<<";"<<scm->ori1[1]<<";"<<scm->ori1[2]<<";"<<scm->ori1[3]<<endl;
				//cerr<<"+++ (normalized) "<<scm->cp1rel<<" || product with "<<se31.orientation<<" is "<<scm->cp1rel*se31.orientation<<endl;
			}
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
