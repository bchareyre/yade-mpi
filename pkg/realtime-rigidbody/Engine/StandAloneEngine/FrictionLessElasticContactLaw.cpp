/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*  basen on SDEC formulas given by Frederic Donze                        *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "FrictionLessElasticContactLaw.hpp"


#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/pkg-common/ClosestFeatures.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>

#include<yade/lib-base/yadeWm3Extra.hpp>



FrictionLessElasticContactLaw::FrictionLessElasticContactLaw () : InteractionSolver()
{
}




void FrictionLessElasticContactLaw::action(MetaBody * ncb)
{
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	Real stiffness = 10000;
	Real viscosity = 10;
	
	InteractionContainer::iterator ii = ncb->transientInteractions->begin();
	InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
	for( ; ii!=iiEnd; ++ii )
	{
		shared_ptr<Interaction> contact = *ii;
		if (contact->isReal())
		{
			int id1 = contact->getId1();
			int id2 = contact->getId2();
	
			shared_ptr<RigidBodyParameters> rb1 = YADE_PTR_CAST<RigidBodyParameters>((*bodies)[id1]->physicalParameters);
			shared_ptr<RigidBodyParameters> rb2 = YADE_PTR_CAST<RigidBodyParameters>((*bodies)[id2]->physicalParameters);
	
			std::vector<std::pair<Vector3r,Vector3r> >::iterator cpi = (YADE_PTR_CAST<ClosestFeatures>(contact->interactionGeometry))->closestsPoints.begin();
			std::vector<std::pair<Vector3r,Vector3r> >::iterator cpiEnd = (YADE_PTR_CAST<ClosestFeatures>(contact->interactionGeometry))->closestsPoints.end();
			Real size = (YADE_PTR_CAST<ClosestFeatures>(contact->interactionGeometry))->closestsPoints.size();
			for( ; cpi!=cpiEnd ; ++cpi)
			{
				Vector3r p1 = (*cpi).first;
				Vector3r p2 = (*cpi).second;
	
				Vector3r p = 0.5*(p1+p2);
	
				Vector3r o1p = (p - rb1->se3.position);
				Vector3r o2p = (p - rb2->se3.position);
	
				Vector3r dir = p2-p1;
				Real l  = dir.Normalize();
				Real elongation  = l*l;
	
				//Vector3r v1 = rb1->velocity+o1p.cross(rb1->angularVelocity);
				//Vector3r v2 = rb2->velocity+o2p.cross(rb2->angularVelocity);
				Vector3r v1 = rb1->velocity+rb1->angularVelocity.Cross(o1p);
				Vector3r v2 = rb2->velocity+rb2->angularVelocity.Cross(o2p);
				Real relativeVelocity = dir.Dot(v2-v1);
				Vector3r f = (elongation*stiffness+relativeVelocity*viscosity)/size*dir;
				ncb->bex.addForce (id1, f);
				ncb->bex.addForce (id2,-f);
				ncb->bex.addTorque(id1, o1p.Cross(f));
				ncb->bex.addTorque(id2,-o2p.Cross(f));

			}
		}
	}
}

YADE_PLUGIN((FrictionLessElasticContactLaw));
YADE_REQUIRE_FEATURE(deprecated)

