/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ElasticContactLaw.hpp"
#include "BodyMacroParameters.hpp"
#include "SpheresContactGeometry.hpp"
#include "SDECLinkGeometry.hpp"
#include "ElasticContactParameters.hpp"
#include "SDECLinkPhysics.hpp"
#include <yade/yade-core/Omega.hpp>
#include <yade/yade-core/MetaBody.hpp>
#include <yade/yade-package-common/Force.hpp>
#include <yade/yade-package-common/Momentum.hpp>
#include <yade/yade-core/PhysicalAction.hpp>


ElasticContactLaw::ElasticContactLaw() : InteractionSolver() , actionForce(new Force) , actionMomentum(new Momentum)
{
	sdecGroupMask=1;
	momentRotationLaw = true;
}


void ElasticContactLaw::registerAttributes()
{
	InteractionSolver::registerAttributes();
	REGISTER_ATTRIBUTE(sdecGroupMask);
	REGISTER_ATTRIBUTE(momentRotationLaw);
}


//FIXME : remove bool first !!!!!
void ElasticContactLaw::action(Body* body)
{
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	Real dt = Omega::instance().getTimeStep();

/// Non Permanents Links												///

	InteractionContainer::iterator ii    = ncb->volatileInteractions->begin();
	InteractionContainer::iterator iiEnd = ncb->volatileInteractions->end();
	for(  ; ii!=iiEnd ; ++ii )
	{
		if ((*ii)->isReal)
		{
			const shared_ptr<Interaction>& contact = *ii;
			int id1 = contact->getId1();
			int id2 = contact->getId2();
			
			if( !( (*bodies)[id1]->getGroupMask() & (*bodies)[id2]->getGroupMask() & sdecGroupMask)  )
				continue; // skip other groups, BTW: this is example of a good usage of 'continue' keyword
	
			BodyMacroParameters* de1 				= dynamic_cast<BodyMacroParameters*>((*bodies)[id1]->physicalParameters.get());
			BodyMacroParameters* de2 				= dynamic_cast<BodyMacroParameters*>((*bodies)[id2]->physicalParameters.get());
			SpheresContactGeometry* currentContactGeometry 	= dynamic_cast<SpheresContactGeometry*>(contact->interactionGeometry.get());
			ElasticContactParameters* currentContactPhysics   	= dynamic_cast<ElasticContactParameters*> (contact->interactionPhysics.get());
			
			Vector3r& shearForce 			= currentContactPhysics->shearForce;
	
			if ( contact->isNew)
				shearForce			= Vector3r(0,0,0);
					
			Real un 				= currentContactGeometry->penetrationDepth;
			currentContactPhysics->normalForce	= currentContactPhysics->kn*un*currentContactGeometry->normal;
	
			Vector3r axis;
			Real angle;
	
	/// Here is the code with approximated rotations 	 ///
			
			axis	 		= currentContactPhysics->prevNormal.cross(currentContactGeometry->normal);
			shearForce 	       -= shearForce.cross(axis);
			angle 			= dt*0.5*currentContactGeometry->normal.dot(de1->angularVelocity+de2->angularVelocity);
			axis 			= angle*currentContactGeometry->normal;
			shearForce 	       -= shearForce.cross(axis);
		
	/// Here is the code with exact rotations 		 ///
	
	// 		Quaternionr q;
	//
	// 		axis					= currentContactPhysics->prevNormal.cross(currentContactGeometry->normal);
	// 		angle					= acos(currentContactGeometry->normal.dot(currentContactPhysics->prevNormal));
	// 		q.fromAngleAxis(angle,axis);
	//
	// 		currentContactPhysics->shearForce	= currentContactPhysics->shearForce*q;
	//
	// 		angle					= dt*0.5*currentContactGeometry->normal.dot(de1->angularVelocity+de2->angularVelocity);
	// 		axis					= currentContactGeometry->normal;
	// 		q.fromAngleAxis(angle,axis);
	// 		currentContactPhysics->shearForce	= q*currentContactPhysics->shearForce;
	
	/// 							 ///
	
			Vector3r x				= currentContactGeometry->contactPoint;
			Vector3r c1x				= (x - de1->se3.position);
			Vector3r c2x				= (x - de2->se3.position);
			Vector3r relativeVelocity		= (de2->velocity+de2->angularVelocity.cross(c2x)) - (de1->velocity+de1->angularVelocity.cross(c1x));
			Vector3r shearVelocity			= relativeVelocity-currentContactGeometry->normal.dot(relativeVelocity)*currentContactGeometry->normal;
			Vector3r shearDisplacement		= shearVelocity*dt;
			shearForce 			       -= currentContactPhysics->ks*shearDisplacement;
	
	// PFC3d SlipModel, is using friction angle. CoulombCriterion
			Real maxFs = currentContactPhysics->normalForce.squaredLength() * std::pow(currentContactPhysics->tangensOfFrictionAngle,2);
			if( shearForce.squaredLength() > maxFs )
			{
				maxFs = Mathr::sqRoot(maxFs) / shearForce.length();
				shearForce *= maxFs;
			}
	////////// PFC3d SlipModel
	
			Vector3r f				= currentContactPhysics->normalForce + shearForce;
			
	// it will be some macro(	body->actionParameters,	ActionType , bodyId )
			static_cast<Force*>   ( ncb->actionParameters->find( id1 , actionForce   ->getClassIndex() ).get() )->force    -= f;
			static_cast<Force*>   ( ncb->actionParameters->find( id2 , actionForce   ->getClassIndex() ).get() )->force    += f;
			
			static_cast<Momentum*>( ncb->actionParameters->find( id1 , actionMomentum->getClassIndex() ).get() )->momentum -= c1x.cross(f);
			static_cast<Momentum*>( ncb->actionParameters->find( id2 , actionMomentum->getClassIndex() ).get() )->momentum += c2x.cross(f);
			
			currentContactPhysics->prevNormal = currentContactGeometry->normal;
		}
	}

}


