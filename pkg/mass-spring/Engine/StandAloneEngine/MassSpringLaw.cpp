/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "MassSpringLaw.hpp"
#include "SpringGeometry.hpp"
#include "SpringPhysics.hpp"
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/Mesh2D.hpp>
#include<yade/pkg-common/ParticleParameters.hpp>
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/Momentum.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>


MassSpringLaw::MassSpringLaw () : InteractionSolver(), actionForce(new Force) , actionMomentum(new Momentum)
{
}


void MassSpringLaw::registerAttributes()
{
	InteractionSolver::registerAttributes();
	REGISTER_ATTRIBUTE(springGroupMask);
}


void MassSpringLaw::action(Body * body)
{
	MetaBody * massSpring = YADE_CAST<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = massSpring->bodies;
	shared_ptr<InteractionContainer>& persistentInteractions = massSpring->persistentInteractions;
	shared_ptr<PhysicalActionContainer>& physicalActions = massSpring->physicalActions;
	

	InteractionContainer::iterator ii    = persistentInteractions->begin();
	InteractionContainer::iterator iiEnd = persistentInteractions->end();
	for(  ; ii!=iiEnd ; ++ii )
	{	
		shared_ptr<Interaction> spring = *ii;
		if (spring->isReal)
		{
			int id1 = spring->getId1();
			int id2 = spring->getId2();
			
			if( !(  (*bodies)[id1]->getGroupMask() & (*bodies)[id2]->getGroupMask() & springGroupMask) )
				continue; // skip other groups
			
			ParticleParameters * p1 = static_cast<ParticleParameters*>((*bodies)[id1]->physicalParameters.get());
			ParticleParameters * p2 = static_cast<ParticleParameters*>((*bodies)[id2]->physicalParameters.get());
			
			SpringPhysics* physics		= static_cast<SpringPhysics*>(spring->interactionPhysics.get());
//			SpringGeometry* geometry	= static_cast<SpringGeometry*>(spring->interactionGeometry.get());
			
			Vector3r v1 = p2->se3.position;
			Vector3r v2 = p1->se3.position;
			
			Real l  = (v2-v1).Length();
			
			Real l0 = physics->initialLength;
			
			Vector3r dir = (v2-v1);
			dir.Normalize();
			
			Real e  = (l-l0)/l0;
			Real relativeVelocity = dir.Dot((p1->velocity-p2->velocity));
			Vector3r f3 = (e*physics->stiffness + relativeVelocity* ( 1.0 - physics->damping )  )*dir;
			
			static_cast<Force*>   ( physicalActions->find( id1 , actionForce->getClassIndex() ).get() )->force    -= f3;
			static_cast<Force*>   ( physicalActions->find( id2 , actionForce->getClassIndex() ).get() )->force    += f3;
		}
	}
	
}

YADE_PLUGIN();
