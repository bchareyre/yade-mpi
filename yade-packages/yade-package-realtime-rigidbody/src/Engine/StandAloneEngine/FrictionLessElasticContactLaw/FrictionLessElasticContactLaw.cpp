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

#include "FrictionLessElasticContactLaw.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-package-common/RigidBodyParameters.hpp>
#include <yade/yade-package-common/ClosestFeatures.hpp>
#include <yade/yade-core/Omega.hpp>
#include <yade/yade-core/MetaBody.hpp>
#include <yade/yade-package-common/Force.hpp>
#include <yade/yade-package-common/Momentum.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

FrictionLessElasticContactLaw::FrictionLessElasticContactLaw () : Engine(), actionForce(new Force) , actionMomentum(new Momentum)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FrictionLessElasticContactLaw::registerAttributes()
{
	Engine::registerAttributes();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FrictionLessElasticContactLaw::action(Body * body)
{
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	Real stiffness = 10000;
	Real viscosity = 10;
	
	InteractionContainer::iterator ii = ncb->volatileInteractions->begin();
	InteractionContainer::iterator iiEnd = ncb->volatileInteractions->end();
	for( ; ii!=iiEnd; ++ii )
	{
		shared_ptr<Interaction> contact = *ii;
		if (contact->isReal)
		{
			int id1 = contact->getId1();
			int id2 = contact->getId2();
	
			shared_ptr<RigidBodyParameters> rb1 = dynamic_pointer_cast<RigidBodyParameters>((*bodies)[id1]->physicalParameters);
			shared_ptr<RigidBodyParameters> rb2 = dynamic_pointer_cast<RigidBodyParameters>((*bodies)[id2]->physicalParameters);
	
			std::vector<std::pair<Vector3r,Vector3r> >::iterator cpi = (dynamic_pointer_cast<ClosestFeatures>(contact->interactionGeometry))->closestsPoints.begin();
			std::vector<std::pair<Vector3r,Vector3r> >::iterator cpiEnd = (dynamic_pointer_cast<ClosestFeatures>(contact->interactionGeometry))->closestsPoints.end();
			Real size = (dynamic_pointer_cast<ClosestFeatures>(contact->interactionGeometry))->closestsPoints.size();
			for( ; cpi!=cpiEnd ; ++cpi)
			{
				Vector3r p1 = (*cpi).first;
				Vector3r p2 = (*cpi).second;
	
				Vector3r p = 0.5*(p1+p2);
	
				Vector3r o1p = (p - rb1->se3.position);
				Vector3r o2p = (p - rb2->se3.position);
	
				Vector3r dir = p2-p1;
				Real l  = dir.normalize();
				Real elongation  = l*l;
	
				//Vector3r v1 = rb1->velocity+o1p.cross(rb1->angularVelocity);
				//Vector3r v2 = rb2->velocity+o2p.cross(rb2->angularVelocity);
				Vector3r v1 = rb1->velocity+rb1->angularVelocity.cross(o1p);
				Vector3r v2 = rb2->velocity+rb2->angularVelocity.cross(o2p);
				Real relativeVelocity = dir.dot(v2-v1);
				Vector3r f = (elongation*stiffness+relativeVelocity*viscosity)/size*dir;
	
				static_cast<Force*>   ( ncb->actionParameters->find( id1 , actionForce   ->getClassIndex() ).get() )->force    += f;
				static_cast<Force*>   ( ncb->actionParameters->find( id2 , actionForce   ->getClassIndex() ).get() )->force    -= f;
			
				static_cast<Momentum*>( ncb->actionParameters->find( id1 , actionMomentum->getClassIndex() ).get() )->momentum += o1p.cross(f);
				static_cast<Momentum*>( ncb->actionParameters->find( id2 , actionMomentum->getClassIndex() ).get() )->momentum -= o2p.cross(f);
			}
		}
	}

// 	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
// 	shared_ptr<BodyContainer>& bodies = ncb->bodies;
// 
// 	Real dt = Omega::instance().getTimeStep();
// 
// 	for( ncb->volatileInteractions->gotoFirst() ; ncb->volatileInteractions->notAtEnd() ; ncb->volatileInteractions->gotoNext() )
// 	{
// 		const shared_ptr<Interaction>& contact = ncb->volatileInteractions->getCurrent();
// 		int id1 = contact->getId1();
// 		int id2 = contact->getId2();
// 
// 		BodyMacroParameters* de1 				= dynamic_cast<BodyMacroParameters*>((*bodies)[id1]->physicalParameters.get());
// 		BodyMacroParameters* de2 				= dynamic_cast<BodyMacroParameters*>((*bodies)[id2]->physicalParameters.get());
// 		MacroMicroContactGeometry* currentContactGeometry 	= dynamic_cast<MacroMicroContactGeometry*>(contact->interactionGeometry.get());
// 		ElasticContactParameters* currentContactPhysics   	= dynamic_cast<ElasticContactParameters*> (contact->interactionPhysics.get());
// 
// 		//Vector3r& shearForce 			= currentContactPhysics->shearForce;
// 
// 		//if ( contact->isNew)
// 		//	shearForce			= Vector3r(0,0,0);
// 
// 		Real un 				= currentContactGeometry->penetrationDepth;
// 		currentContactPhysics->normalForce	= currentContactPhysics->kn*un*currentContactGeometry->normal;
// 
// 		Vector3r axis;
// 		Real angle;
// 
// 
// 		//axis	 		= currentContactPhysics->prevNormal.cross(currentContactGeometry->normal);
// 		//shearForce 	       -= shearForce.cross(axis);
// 		//angle 			= dt*0.5*currentContactGeometry->normal.dot(de1->angularVelocity+de2->angularVelocity);
// 		//axis 			= angle*currentContactGeometry->normal;
// 		//shearForce 	       -= shearForce.cross(axis);
// 
// 		//Vector3r x				= currentContactGeometry->contactPoint;
// 		//Vector3r c1x				= (x - de1->se3.position);
// 		//Vector3r c2x				= (x - de2->se3.position);
// 		//Vector3r relativeVelocity		= (de2->velocity+de2->angularVelocity.cross(c2x)) - (de1->velocity+de1->angularVelocity.cross(c1x));
// 		//Vector3r shearVelocity			= relativeVelocity-currentContactGeometry->normal.dot(relativeVelocity)*currentContactGeometry->normal;
// 		//Vector3r shearDisplacement		= shearVelocity*dt;
// 		//shearForce 			       -= currentContactPhysics->ks*shearDisplacement;
// 
// 		//Real maxFs = currentContactPhysics->normalForce.squaredLength() * std::pow(currentContactPhysics->tangensOfFrictionAngle,2);
// 		//if( shearForce.squaredLength() > maxFs )
// 		//{
// 		//	maxFs = Mathr::sqRoot(maxFs) / shearForce.length();
// 		//	shearForce *= maxFs;
// 		//}
// 
// 		Vector3r f = currentContactPhysics->normalForce;// + shearForce;
// 
// 		static_cast<Force*>   ( ncb->actionParameters->find( id1 , actionForce   ->getClassIndex() ).get() )->force    -= f;
// 		static_cast<Force*>   ( ncb->actionParameters->find( id2 , actionForce   ->getClassIndex() ).get() )->force    += f;
// 
// 		static_cast<Momentum*>( ncb->actionParameters->find( id1 , actionMomentum->getClassIndex() ).get() )->momentum -= c1x.cross(f);
// 		static_cast<Momentum*>( ncb->actionParameters->find( id2 , actionMomentum->getClassIndex() ).get() )->momentum += c2x.cross(f);
// 
// 		//currentContactPhysics->prevNormal = currentContactGeometry->normal;
// 	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
