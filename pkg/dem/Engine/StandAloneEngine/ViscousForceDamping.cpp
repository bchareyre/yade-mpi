/*************************************************************************
*  Copyright (C) 2008 by Feng Chen  (fchen3@gmail.com)                   *
*  Department of Civil and Environmental Engineering                     *
*  223 Perkins Hall                                                      *
*  University of Tennessee, Knoxville, 37996                             *
*  http://fchen3.googlepages.com/home                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ViscousForceDamping.hpp"
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>

ViscousForceDamping::ViscousForceDamping() : InteractionSolver() ,betaNormal(0.0), betaShear(0.0)
{
	sdecGroupMask=1;
	momentRotationLaw = true;
	///====================================
	///betaNormal = 0.0;
	///betaShear = 0.0;
}


void ViscousForceDamping::registerAttributes()
{
	InteractionSolver::registerAttributes();
	REGISTER_ATTRIBUTE(sdecGroupMask);
	REGISTER_ATTRIBUTE(momentRotationLaw);
	REGISTER_ATTRIBUTE(betaNormal);
	REGISTER_ATTRIBUTE(betaShear);
}


//FIXME : remove bool first !!!!!
void ViscousForceDamping::action(Body* body)
{
	MetaBody * ncb = YADE_CAST<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	Real dt = Omega::instance().getTimeStep();

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
			
			if( !( (*bodies)[id1]->getGroupMask() & (*bodies)[id2]->getGroupMask() & sdecGroupMask)  )
				continue; // skip other groups, BTW: this is example of a good usage of 'continue' keyword
	
			BodyMacroParameters* de1 				= YADE_CAST<BodyMacroParameters*>((*bodies)[id1]->physicalParameters.get());
			BodyMacroParameters* de2 				= YADE_CAST<BodyMacroParameters*>((*bodies)[id2]->physicalParameters.get());
			SpheresContactGeometry* currentContactGeometry		= YADE_CAST<SpheresContactGeometry*>(contact->interactionGeometry.get());
			ElasticContactInteraction* currentContactPhysics   	= YADE_CAST<ElasticContactInteraction*> (contact->interactionPhysics.get());
			
			Vector3r& shearForce 			= currentContactPhysics->shearForce;
	
			if ( contact->isNew)
				shearForce			= Vector3r(0,0,0);
					
			Real un 				= currentContactGeometry->penetrationDepth;
			currentContactPhysics->normalForce	= currentContactPhysics->kn*std::max(un,(Real) 0)*currentContactGeometry->normal;
	
			Vector3r axis;
			Real angle;
	
	/// Here is the code with approximated rotations 	 ///
			
			axis	 		= currentContactPhysics->prevNormal.Cross(currentContactGeometry->normal);
			shearForce 	       -= shearForce.Cross(axis);
			angle 			= dt*0.5*currentContactGeometry->normal.Dot(de1->angularVelocity+de2->angularVelocity);
			axis 			= angle*currentContactGeometry->normal;
			shearForce 	       -= shearForce.Cross(axis);
		
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
	
	//===============================================================================
	//Viscous damping
			Vector3r normalVelocity			= relativeVelocity - shearVelocity;
			
			Real m1 = de1->mass;
			Real m2 = de2->mass;
			Real mBar = m1*m2/(m1+m2);
			if (!(*bodies)[id1]->isDynamic)	///id1 is a static wall
				mBar = m2;
			if (!(*bodies)[id2]->isDynamic)	///id2 is a static wall
				mBar = m1;
			
//			std::cerr << "kn:" << currentContactPhysics->kn << endl;
//			std::cerr << "ks:" << currentContactPhysics->ks << endl;
			
//			std::cerr << "mBar:" << mBar << endl;

			Real cCritNormal = 2.0*sqrt(mBar*currentContactPhysics->kn);
			Real cCritShear  = 2.0*sqrt(mBar*currentContactPhysics->ks);
			
			Real cNormal = betaNormal*cCritNormal;
			Real cShear = betaShear*cCritShear;
			Vector3r normalDampingForce = -cNormal*normalVelocity;
			Vector3r shearDampingForce = -cShear*shearVelocity;
			Vector3r viscousDampingForce	= normalDampingForce + shearDampingForce;
			
//	Add forces
			ncb->bex.addForce (id1,-viscousDampingForce);
			ncb->bex.addForce (id2,+viscousDampingForce);
			ncb->bex.addTorque(id1,-c1x.Cross(viscousDampingForce));
			ncb->bex.addTorque(id2, c2x.Cross(viscousDampingForce));
			currentContactPhysics->prevNormal = currentContactGeometry->normal;
		}
	}

}


