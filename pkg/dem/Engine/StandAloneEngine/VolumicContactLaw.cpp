/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/Momentum.hpp>
#include<yade/core/PhysicalAction.hpp>

#include <yade/pkg-common/InteractingSphere.hpp>

#include "VolumicContactLaw.hpp"

#include "TesselationWrapper.h"




VolumicContactLaw::VolumicContactLaw() : InteractionSolver() , actionForce(new Force) , actionMomentum(new Momentum)
{
	sdecGroupMask=1;
	momentRotationLaw = true;
	actionForceIndex = actionForce->getClassIndex();
	actionMomentumIndex = actionMomentum->getClassIndex();
	
}


void VolumicContactLaw::registerAttributes()
{
	InteractionSolver::registerAttributes();
	REGISTER_ATTRIBUTE(sdecGroupMask);
	REGISTER_ATTRIBUTE(momentRotationLaw);
}


void VolumicContactLaw::action(MetaBody* ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	Real dt = Omega::instance().getTimeStep();


	//BEGIN VORONOI TESSELATION
	TesselationWrapper T1;	
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for(  ; bi!=biEnd ; ++bi )
	{
		if ((*bi)->isDynamic) {//means "is it a sphere (not a wall)"
			const InteractingSphere* s = YADE_CAST<InteractingSphere*>((*bi)->interactingGeometry.get());
			const RigidBodyParameters* p = YADE_CAST<RigidBodyParameters*>((*bi)->physicalParameters.get());
			T1.insert(p->se3.position[0],p->se3.position[1],p->se3.position[2], s->radius, (*bi)->getId());
		}
	}
	T1.ComputeVolumes();
	//ENDOF VORONOI TESSELATION
	
			
/// Non Permanents Links												///

	InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
	InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
	for(  ; ii!=iiEnd ; ++ii )
	{
		if ((*ii)->isReal)
		{
			const shared_ptr<Interaction>& contact = *ii;
			int id1 = contact->getId1();
			int id2 = contact->getId2();
			
			if( !( (*bodies)[id1]->getGroupMask() & (*bodies)[id2]->getGroupMask() & sdecGroupMask) ) continue;

			const SpheresContactGeometry*    currentContactGeometry= YADE_CAST<SpheresContactGeometry*>(contact->interactionGeometry.get());
			ElasticContactInteraction* currentContactPhysics = YADE_CAST<ElasticContactInteraction*> (contact->interactionPhysics.get());
			if((!currentContactGeometry)||(!currentContactPhysics)) continue;
	
			BodyMacroParameters* de1 				= YADE_CAST<BodyMacroParameters*>((*bodies)[id1]->physicalParameters.get());
			BodyMacroParameters* de2 				= YADE_CAST<BodyMacroParameters*>((*bodies)[id2]->physicalParameters.get());
			
			Vector3r& shearForce 			= currentContactPhysics->shearForce;
	
			if (contact->isNew) shearForce=Vector3r(0,0,0);
					
			Real un=currentContactGeometry->penetrationDepth;
			
			
// 			currentContactPhysics->normalForce=currentContactPhysics->kn*std::max(un,(Real) 0) *currentContactGeometry->normal;
									
			//BEGIN USE VOLUME
			Real vol1 = ((*bodies)[id1]->isDynamic ? T1.Volume(id1) : 0);
			Real vol2 = ((*bodies)[id2]->isDynamic ? T1.Volume(id2) : 0);
			Real solid1 = ((*bodies)[id1]->isDynamic ? 4.18 * (pow(currentContactGeometry->radius1,3)) : 0);
			Real solid2 = ((*bodies)[id2]->isDynamic ? 4.18 * (pow(currentContactGeometry->radius2,3)) : 0);
			Real compacity = (solid1+solid2)/(vol1+vol2);
			//cerr << "compacity " << id1 << "/" << id2 << " = " << compacity << endl;
			
			currentContactPhysics->normalForce=currentContactPhysics->kn*std::max(un,(Real) 0) *currentContactGeometry->normal //loi normale
			* max((Real) 1,pow(compacity*1.42, 3)); // + effet volumique
			//ENDOF USE VOLUME
	
			Vector3r axis;
			Real angle;
	
	/// Here is the code with approximated rotations 	 ///
			
			axis = currentContactPhysics->prevNormal.Cross(currentContactGeometry->normal);
			shearForce -= shearForce.Cross(axis);
			angle = dt*0.5*currentContactGeometry->normal.Dot(de1->angularVelocity+de2->angularVelocity);
			axis = angle*currentContactGeometry->normal;
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
	
	/// 							 ///
	
			Vector3r x				= currentContactGeometry->contactPoint;
			Vector3r c1x				= (x - de1->se3.position);
			Vector3r c2x				= (x - de2->se3.position);
			Vector3r relativeVelocity		= (de2->velocity+de2->angularVelocity.Cross(c2x)) - (de1->velocity+de1->angularVelocity.Cross(c1x));
			Vector3r shearVelocity			= relativeVelocity-currentContactGeometry->normal.Dot(relativeVelocity)*currentContactGeometry->normal;
			Vector3r shearDisplacement		= shearVelocity*dt;
			shearForce 			       -= currentContactPhysics->ks*shearDisplacement;
	
	// PFC3d SlipModel, is using friction angle. CoulombCriterion
			Real maxFs = currentContactPhysics->normalForce.SquaredLength() * std::pow(currentContactPhysics->tangensOfFrictionAngle,2);
			if( shearForce.SquaredLength() > maxFs )
			{
				maxFs = Mathr::Sqrt(maxFs) / shearForce.Length();
				shearForce *= maxFs;
			}
	////////// PFC3d SlipModel
	
			Vector3r f				= currentContactPhysics->normalForce + shearForce;
			
	// it will be some macro(	body->physicalActions,	ActionType , bodyId )
			static_cast<Force*>   ( ncb->physicalActions->find( id1 , actionForceIndex).get() )->force    -= f;
			static_cast<Force*>   ( ncb->physicalActions->find( id2 , actionForceIndex ).get() )->force    += f;
			
			static_cast<Momentum*>( ncb->physicalActions->find( id1 , actionMomentumIndex ).get() )->momentum -= c1x.Cross(f);
			static_cast<Momentum*>( ncb->physicalActions->find( id2 , actionMomentumIndex ).get() )->momentum += c2x.Cross(f);
			
			currentContactPhysics->prevNormal = currentContactGeometry->normal;
		}
	}

}


YADE_PLUGIN();
