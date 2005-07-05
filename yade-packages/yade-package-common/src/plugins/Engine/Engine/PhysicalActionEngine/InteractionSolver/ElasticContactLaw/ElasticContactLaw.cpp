/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "ElasticContactLaw.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-common/BodyMacroParameters.hpp>
#include <yade/yade-common/MacroMicroContactGeometry.hpp>
#include <yade/yade-common/SDECLinkGeometry.hpp>
#include <yade/yade-common/ElasticContactParameters.hpp>
#include <yade/yade-common/SDECLinkPhysics.hpp>
#include <yade/yade-core/Omega.hpp>
#include <yade/yade-core/MetaBody.hpp>
#include <yade/yade-common/Force.hpp>
#include <yade/yade-common/Momentum.hpp>
#include <yade/yade-core/PhysicalAction.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

ElasticContactLaw::ElasticContactLaw() : InteractionSolver() , actionForce(new Force) , actionMomentum(new Momentum)
{
	sdecGroupMask=1;
	momentRotationLaw = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ElasticContactLaw::registerAttributes()
{
	InteractionSolver::registerAttributes();
	REGISTER_ATTRIBUTE(sdecGroupMask);
	REGISTER_ATTRIBUTE(momentRotationLaw);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//FIXME : remove bool first !!!!!
void ElasticContactLaw::calculateForces(Body* body)
{
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	Real dt = Omega::instance().getTimeStep();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Non Permanents Links												///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	for( ncb->volatileInteractions->gotoFirst() ; ncb->volatileInteractions->notAtEnd() ; ncb->volatileInteractions->gotoNext() )
	{
		const shared_ptr<Interaction>& contact = ncb->volatileInteractions->getCurrent();
		int id1 = contact->getId1();
		int id2 = contact->getId2();
		
		if( !( (*bodies)[id1]->getGroupMask() & (*bodies)[id2]->getGroupMask() & sdecGroupMask)  )
			continue; // skip other groups, BTW: this is example of a good usage of 'continue' keyword

		BodyMacroParameters* de1 				= dynamic_cast<BodyMacroParameters*>((*bodies)[id1]->physicalParameters.get());
		BodyMacroParameters* de2 				= dynamic_cast<BodyMacroParameters*>((*bodies)[id2]->physicalParameters.get());
		MacroMicroContactGeometry* currentContactGeometry 	= dynamic_cast<MacroMicroContactGeometry*>(contact->interactionGeometry.get());
		ElasticContactParameters* currentContactPhysics   	= dynamic_cast<ElasticContactParameters*> (contact->interactionPhysics.get());
		
		Vector3r& shearForce 			= currentContactPhysics->shearForce;

		if ( contact->isNew)
			shearForce			= Vector3r(0,0,0);
				
		Real un 				= currentContactGeometry->penetrationDepth;
		currentContactPhysics->normalForce	= currentContactPhysics->kn*un*currentContactGeometry->normal;

		Vector3r axis;
		Real angle;

////////////////////////////////////////////////////////////
/// Here is the code with approximated rotations 	 ///
////////////////////////////////////////////////////////////
		
		axis	 		= currentContactPhysics->prevNormal.cross(currentContactGeometry->normal);
		shearForce 	       -= shearForce.cross(axis);
		angle 			= dt*0.5*currentContactGeometry->normal.dot(de1->angularVelocity+de2->angularVelocity);
		axis 			= angle*currentContactGeometry->normal;
		shearForce 	       -= shearForce.cross(axis);
	
////////////////////////////////////////////////////////////
/// Here is the code with exact rotations 		 ///
////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////
/// 							 ///
////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

