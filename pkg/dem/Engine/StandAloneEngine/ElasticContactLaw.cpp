/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"ElasticContactLaw.hpp"
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/Momentum.hpp>
#include<yade/core/PhysicalAction.hpp>

#include<yade/extra/Shop.hpp>

ElasticContactLaw2::ElasticContactLaw2(){
	Shop::Bex::initCache();
	isCohesive=true;
}

ElasticContactLaw2::~ElasticContactLaw2(){}

void ElasticContactLaw2::action(MetaBody* rb){
	Real /* bending stiffness */ kb=1e7, /* torsion stiffness */ ktor=1e8;
	FOREACH(shared_ptr<Interaction> i, *rb->transientInteractions){
		if(!i->isReal) continue;
		shared_ptr<SpheresContactGeometry> contGeom=YADE_PTR_CAST<SpheresContactGeometry>(i->interactionGeometry);
		shared_ptr<ElasticContactInteraction> contPhys=YADE_PTR_CAST<ElasticContactInteraction>(i->interactionPhysics);
		assert(contGeom); assert(contPhys);
		if(!contGeom->hasShear) throw runtime_error("SpheresContactGeometry::hasShear must be true for ElasticContactLaw2");
		Real Fn=contPhys->kn*contGeom->displacementN(); // scalar normal force; displacementN()>=0 ≡ elongation of the contact
		if(!isCohesive && contGeom->displacementN()>0){ cerr<<"deleting"<<endl; /* delete the interaction */ i->isReal=false; continue;}
		contPhys->normalForce=Fn*contGeom->normal;
		//contGeom->relocateContactPoints();
		//contGeom->slipToDisplacementTMax(max(0.,(-Fn*contPhys->tangensOfFrictionAngle)/contPhys->ks)); // limit shear displacement -- Coulomb criterion
		contPhys->shearForce=contPhys->ks*contGeom->displacementT();
		Vector3r force=contPhys->shearForce+contPhys->normalForce;
		Shop::applyForceAtContactPoint(force,contGeom->contactPoint,i->getId1(),contGeom->pos1,i->getId2(),contGeom->pos2,rb);

		Vector3r bendAbs; Real torsionAbs; contGeom->bendingTorsionAbs(bendAbs,torsionAbs);
		Shop::Bex::momentum(i->getId1(),rb)+=contGeom->normal*torsionAbs*ktor;
		Shop::Bex::momentum(i->getId2(),rb)-=contGeom->normal*torsionAbs*ktor;
		Shop::Bex::momentum(i->getId1(),rb)+=bendAbs*kb;
		Shop::Bex::momentum(i->getId2(),rb)-=bendAbs*kb;
	}
}



ElasticContactLaw::ElasticContactLaw() : InteractionSolver() , actionForce(new Force) , actionMomentum(new Momentum)
{
	sdecGroupMask=1;
	momentRotationLaw = true;
	actionForceIndex = actionForce->getClassIndex();
	actionMomentumIndex = actionMomentum->getClassIndex();
}


void ElasticContactLaw::registerAttributes()
{
	InteractionSolver::registerAttributes();
	REGISTER_ATTRIBUTE(sdecGroupMask);
	REGISTER_ATTRIBUTE(momentRotationLaw);
}


void ElasticContactLaw::action(MetaBody* ncb)
{
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
			
			if( !( (*bodies)[id1]->getGroupMask() & (*bodies)[id2]->getGroupMask() & sdecGroupMask) ) continue;

			SpheresContactGeometry*    currentContactGeometry= YADE_CAST<SpheresContactGeometry*>(contact->interactionGeometry.get());
			ElasticContactInteraction* currentContactPhysics = YADE_CAST<ElasticContactInteraction*> (contact->interactionPhysics.get());
			if((!currentContactGeometry)||(!currentContactPhysics)) continue;
			// delete interaction where spheres don't touch
			if(currentContactGeometry->penetrationDepth<0){ (*ii)->isReal=false; continue; }
	
			BodyMacroParameters* de1 				= YADE_CAST<BodyMacroParameters*>((*bodies)[id1]->physicalParameters.get());
			BodyMacroParameters* de2 				= YADE_CAST<BodyMacroParameters*>((*bodies)[id2]->physicalParameters.get());

			bool isDynamic1 = (*bodies)[id1]->isDynamic;
			bool isDynamic2 = (*bodies)[id2]->isDynamic;

			Vector3r& shearForce 			= currentContactPhysics->shearForce;
	
			if (contact->isNew) shearForce=Vector3r(0,0,0);
					
			Real un=currentContactGeometry->penetrationDepth;
			currentContactPhysics->normalForce=currentContactPhysics->kn*std::max(un,(Real) 0)*currentContactGeometry->normal;
	
			Vector3r axis;
			Real angle;

	/// Here is the code with approximated rotations 	 ///
			


			axis = currentContactPhysics->prevNormal.Cross(currentContactGeometry->normal);
			shearForce -= shearForce.Cross(axis);
			//angle = dt*0.5*currentContactGeometry->normal.Dot(de1->angularVelocity+de2->angularVelocity);
				//FIXME: if one body is kinematic then assumed its rotation centre does not lies along the normal
				//(i.e. virtual sphere, which replaces this kinematic body on contact, does not rotate)
			Vector3r summaryAngularVelocity(0,0,0);
			if (isDynamic1) summaryAngularVelocity += de1->angularVelocity;
			if (isDynamic2) summaryAngularVelocity += de2->angularVelocity;
			angle = dt*0.5*currentContactGeometry->normal.Dot(summaryAngularVelocity);
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
			 /// The following definition of c1x and c2x is to avoid "granular ratcheting" 
			///  (see F. ALONSO-MARROQUIN, R. GARCIA-ROJO, H.J. HERRMANN, 
			///   "Micro-mechanical investigation of granular ratcheting, in Cyclic Behaviour of Soils and Liquefaction Phenomena",
			///   ed. T. Triantafyllidis (Balklema, London, 2004), p. 3-10 - and a lot more papers from the same authors)
            		Vector3r _c1x_	= (isDynamic1) ? currentContactGeometry->radius1*currentContactGeometry->normal : x - de1->zeroPoint;
            		Vector3r _c2x_	= (isDynamic2) ? -currentContactGeometry->radius2*currentContactGeometry->normal : x - de2->zeroPoint;
			Vector3r relativeVelocity		= (de2->velocity+de2->angularVelocity.Cross(_c2x_)) - (de1->velocity+de1->angularVelocity.Cross(_c1x_));
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
