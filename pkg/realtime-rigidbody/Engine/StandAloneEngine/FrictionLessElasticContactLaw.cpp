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
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/Momentum.hpp>

#include<yade/lib-base/yadeWm3Extra.hpp>



FrictionLessElasticContactLaw::FrictionLessElasticContactLaw () : InteractionSolver(), actionForce(new Force) , actionMomentum(new Momentum)
{
}


void FrictionLessElasticContactLaw::registerAttributes()
{
	InteractionSolver::registerAttributes();
}


void FrictionLessElasticContactLaw::action(Body * body)
{
	MetaBody * ncb = YADE_CAST<MetaBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	Real stiffness = 10000;
	Real viscosity = 10;
	
	InteractionContainer::iterator ii = ncb->transientInteractions->begin();
	InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
	for( ; ii!=iiEnd; ++ii )
	{
		shared_ptr<Interaction> contact = *ii;
		if (contact->isReal)
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
	
				static_cast<Force*>   ( ncb->physicalActions->find( id1 , actionForce   ->getClassIndex() ).get() )->force    += f;
				static_cast<Force*>   ( ncb->physicalActions->find( id2 , actionForce   ->getClassIndex() ).get() )->force    -= f;
			
				static_cast<Momentum*>( ncb->physicalActions->find( id1 , actionMomentum->getClassIndex() ).get() )->momentum += o1p.Cross(f);
				static_cast<Momentum*>( ncb->physicalActions->find( id2 , actionMomentum->getClassIndex() ).get() )->momentum -= o2p.Cross(f);
			}
		}
	}

// 	MetaBody * ncb = YADE_CAST<MetaBody*>(body);
// 	shared_ptr<BodyContainer>& bodies = ncb->bodies;
// 
// 	Real dt = Omega::instance().getTimeStep();
// 
// 	for( ncb->transientInteractions->gotoFirst() ; ncb->transientInteractions->notAtEnd() ; ncb->transientInteractions->gotoNext() )
// 	{
// 		const shared_ptr<Interaction>& contact = ncb->transientInteractions->getCurrent();
// 		int id1 = contact->getId1();
// 		int id2 = contact->getId2();
// 
// 		BodyMacroParameters* de1 				= YADE_CAST<BodyMacroParameters*>((*bodies)[id1]->physicalParameters.get());
// 		BodyMacroParameters* de2 				= YADE_CAST<BodyMacroParameters*>((*bodies)[id2]->physicalParameters.get());
// 		SpheresContactGeometry* currentContactGeometry 	= YADE_CAST<SpheresContactGeometry*>(contact->interactionGeometry.get());
// 		ElasticContactInteraction* currentContactPhysics   	= YADE_CAST<ElasticContactInteraction*> (contact->interactionPhysics.get());
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
// 		static_cast<Force*>   ( ncb->physicalActions->find( id1 , actionForce   ->getClassIndex() ).get() )->force    -= f;
// 		static_cast<Force*>   ( ncb->physicalActions->find( id2 , actionForce   ->getClassIndex() ).get() )->force    += f;
// 
// 		static_cast<Momentum*>( ncb->physicalActions->find( id1 , actionMomentum->getClassIndex() ).get() )->momentum -= c1x.cross(f);
// 		static_cast<Momentum*>( ncb->physicalActions->find( id2 , actionMomentum->getClassIndex() ).get() )->momentum += c2x.cross(f);
// 
// 		//currentContactPhysics->prevNormal = currentContactGeometry->normal;
// 	}
}

