/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*									 *
*  Copyright (C) 2008 by Sergei Dorofeenko                               *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"SimpleViscoelasticContactLaw.hpp"
#include<yade/pkg-dem/SimpleViscoelasticBodyParameters.hpp>
#include<yade/pkg-dem/SimpleViscoelasticInteraction.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/Momentum.hpp>
#include<yade/core/PhysicalAction.hpp>


SimpleViscoelasticContactLaw::SimpleViscoelasticContactLaw() : InteractionSolver() , actionForce(new Force) , actionMomentum(new Momentum)
{
//	sdecGroupMask=1;
	momentRotationLaw = true;
	actionForceIndex = actionForce->getClassIndex();
	actionMomentumIndex = actionMomentum->getClassIndex();
}


void SimpleViscoelasticContactLaw::registerAttributes()
{
	InteractionSolver::registerAttributes();
//	REGISTER_ATTRIBUTE(sdecGroupMask);
	REGISTER_ATTRIBUTE(momentRotationLaw);
}


void SimpleViscoelasticContactLaw::action(MetaBody* ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	Real dt = Omega::instance().getTimeStep();

/// Non Permanents Links	///

	InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
	InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
	for(  ; ii!=iiEnd ; ++ii )
	{
		if ((*ii)->isReal)
		{
			const shared_ptr<Interaction>& contact = *ii;
			int id1 = contact->getId1();
			int id2 = contact->getId2();
			
//			if( !( (*bodies)[id1]->getGroupMask() & (*bodies)[id2]->getGroupMask() & sdecGroupMask) ) continue;

			SpheresContactGeometry*    currentContactGeometry= YADE_CAST<SpheresContactGeometry*>(contact->interactionGeometry.get());
			SimpleViscoelasticInteraction*  currentContactPhysics = YADE_CAST<SimpleViscoelasticInteraction*> (contact->interactionPhysics.get());
			if((!currentContactGeometry)||(!currentContactPhysics)) continue;
	
			SimpleViscoelasticBodyParameters* de1	= YADE_CAST<SimpleViscoelasticBodyParameters*>((*bodies)[id1]->physicalParameters.get());
			SimpleViscoelasticBodyParameters* de2	= YADE_CAST<SimpleViscoelasticBodyParameters*>((*bodies)[id2]->physicalParameters.get());
			
			Vector3r& normal			= currentContactGeometry->normal;
			Vector3r x				= currentContactGeometry->contactPoint;
			Vector3r c1x				= (x - de1->se3.position);
			Vector3r c2x				= (x - de2->se3.position);
			Vector3r relativeVelocity		= (de2->velocity+de2->angularVelocity.Cross(c2x)) - (de1->velocity+de1->angularVelocity.Cross(c1x));
			Real     normalVelocity			= normal.Dot(relativeVelocity);
			Vector3r shearVelocity			= relativeVelocity-normalVelocity*normal;

			Real kn=currentContactPhysics->kn;
			Real cn=currentContactPhysics->cn;
			Real un=currentContactGeometry->penetrationDepth;
			currentContactPhysics->normalForce = ( kn * un - cn * normalVelocity ) * normal;
	
			Vector3r& shearForce 			= currentContactPhysics->shearForce;

			Vector3r axis;
			Real angle;
	
	/// Here is the code with approximated rotations 	 ///
			
			axis = currentContactPhysics->prevNormal.Cross(normal);
			shearForce -= shearForce.Cross(axis);
			angle = dt*0.5*normal.Dot(de1->angularVelocity+de2->angularVelocity);
			axis = angle*normal;
			shearForce -= shearForce.Cross(axis);
		
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
	
	
			Real ks = currentContactPhysics->ks;
			Real cs = currentContactPhysics->cs;
			shearForce 			       -= (ks*dt+cs)*shearVelocity;
	
	// PFC3d SlipModel, is using friction angle. CoulombCriterion
			Real maxFs = currentContactPhysics->normalForce.SquaredLength() * std::pow(currentContactPhysics->mu,2);
			if( shearForce.SquaredLength() > maxFs )
			{
				maxFs = Mathr::Sqrt(maxFs) / shearForce.Length();
				shearForce *= maxFs;
			}
	////////// PFC3d SlipModel
	
			Vector3r f				= currentContactPhysics->normalForce + shearForce;
			
	// it will be some macro(	body->physicalActions,	ActionType , bodyId )
			static_cast<Force*>   ( ncb->physicalActions->find( id1 , actionForceIndex).get() )->force    -= f;
			static_cast<Force*>   ( ncb->physicalActions->find( id2 , actionForceIndex ).get() )->force   += f;
			
			static_cast<Momentum*>( ncb->physicalActions->find( id1 , actionMomentumIndex ).get() )->momentum -= c1x.Cross(f);
			static_cast<Momentum*>( ncb->physicalActions->find( id2 , actionMomentumIndex ).get() )->momentum += c2x.Cross(f);
			
			currentContactPhysics->prevNormal = normal;
		}
	}
}


YADE_PLUGIN();
