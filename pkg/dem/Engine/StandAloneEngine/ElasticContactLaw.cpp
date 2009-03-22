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
		#ifdef BEX_CONTAINER
			rb->bex.addTorque(i->getId1(), contGeom->normal*torsionAbs*ktor+bendAbs*kb);
			rb->bex.addTorque(i->getId2(),-contGeom->normal*torsionAbs*ktor-bendAbs*kb);
		#else
			Shop::Bex::momentum(i->getId1(),rb)+=contGeom->normal*torsionAbs*ktor;
			Shop::Bex::momentum(i->getId2(),rb)-=contGeom->normal*torsionAbs*ktor;
			Shop::Bex::momentum(i->getId1(),rb)+=bendAbs*kb;
			Shop::Bex::momentum(i->getId2(),rb)-=bendAbs*kb;
		#endif
	}
}



ElasticContactLaw::ElasticContactLaw() : InteractionSolver() , actionForce(new Force) , actionMomentum(new Momentum)
{
	sdecGroupMask=1;
	momentRotationLaw = true;
	actionForceIndex = actionForce->getClassIndex();
	actionMomentumIndex = actionMomentum->getClassIndex();
	#ifdef SCG_SHEAR
		useShear=false;
	#endif
}


void ElasticContactLaw::registerAttributes()
{
	InteractionSolver::registerAttributes();
	REGISTER_ATTRIBUTE(sdecGroupMask);
	REGISTER_ATTRIBUTE(momentRotationLaw);
	#ifdef SCG_SHEAR
		REGISTER_ATTRIBUTE(useShear);
	#endif
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

			Vector3r& shearForce 			= currentContactPhysics->shearForce;
	
			if (contact->isNew) shearForce=Vector3r(0,0,0);
					
			Real un=currentContactGeometry->penetrationDepth;
			currentContactPhysics->normalForce=currentContactPhysics->kn*std::max(un,(Real) 0)*currentContactGeometry->normal;
	
			// the same as under #else, but refactored
			#ifdef SCG_SHEAR
				if(useShear){
					currentContactGeometry->updateShear(de1,de2,dt);
					shearForce=currentContactPhysics->ks*currentContactGeometry->shear;
				} else {
					currentContactGeometry->updateShearForce(shearForce,currentContactPhysics->ks,currentContactPhysics->prevNormal,de1,de2,dt);
				}
			#else
				currentContactGeometry->updateShearForce(shearForce,currentContactPhysics->ks,currentContactPhysics->prevNormal,de1,de2,dt);
			#endif
			
			// PFC3d SlipModel, is using friction angle. CoulombCriterion
			Real maxFs = currentContactPhysics->normalForce.SquaredLength() * std::pow(currentContactPhysics->tangensOfFrictionAngle,2);
			if( shearForce.SquaredLength() > maxFs )
			{
				Real ratio = Mathr::Sqrt(maxFs) / shearForce.Length();
				shearForce *= ratio;
				#ifdef SCG_SHEAR
					// in this case, total shear displacement must be updated as well
					if(useShear) currentContactGeometry->shear*=ratio;
				#endif
			}
			////////// PFC3d SlipModel
	
			Vector3r f=currentContactPhysics->normalForce + shearForce;
			Vector3r _c1x(currentContactGeometry->contactPoint-de1->se3.position),
				_c2x(currentContactGeometry->contactPoint-de2->se3.position);
			#ifdef BEX_CONTAINER
				ncb->bex.addForce (id1,-f);
				ncb->bex.addForce (id2,+f);
				ncb->bex.addTorque(id1,-_c1x.Cross(f));
				ncb->bex.addTorque(id2, _c2x.Cross(f));
			#else
				static_cast<Force*>   ( ncb->physicalActions->find( id1 , actionForceIndex).get() )->force    -= f;
				static_cast<Force*>   ( ncb->physicalActions->find( id2 , actionForceIndex ).get() )->force    += f;
				static_cast<Momentum*>( ncb->physicalActions->find( id1 , actionMomentumIndex ).get() )->momentum -= _c1x.Cross(f);
				static_cast<Momentum*>( ncb->physicalActions->find( id2 , actionMomentumIndex ).get() )->momentum += _c2x.Cross(f);
			#endif
			
			currentContactPhysics->prevNormal = currentContactGeometry->normal;
		}
	}
}


YADE_PLUGIN();
