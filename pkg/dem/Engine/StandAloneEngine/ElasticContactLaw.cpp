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

#include<yade/extra/Shop.hpp>

YADE_PLUGIN("ElasticContactLaw2","ef2_Spheres_Elastic_ElasticLaw","ElasticContactLaw");

ElasticContactLaw2::ElasticContactLaw2(){
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
		rb->bex.addTorque(i->getId1(), contGeom->normal*torsionAbs*ktor+bendAbs*kb);
		rb->bex.addTorque(i->getId2(),-contGeom->normal*torsionAbs*ktor-bendAbs*kb);
	}
}




ElasticContactLaw::ElasticContactLaw() : InteractionSolver()
{
	sdecGroupMask=1;
	momentRotationLaw = true;
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


void ElasticContactLaw::action(MetaBody* rootBody)
{
	if(!functor) functor=shared_ptr<ef2_Spheres_Elastic_ElasticLaw>(new ef2_Spheres_Elastic_ElasticLaw);
	functor->momentRotationLaw=momentRotationLaw;
	functor->sdecGroupMask=sdecGroupMask;
	#ifdef SCG_SHEAR
		functor->useShear=useShear;
	#endif
	FOREACH(const shared_ptr<Interaction>& I, *rootBody->interactions){
		if(!I->isReal) continue;
		#ifdef YADE_DEBUG
			// these checks would be redundant in the functor (ConstitutiveLawDispatcher does that already)
			if(!dynamic_cast<SpheresContactGeometry*>(I->interactionGeometry.get()) || !dynamic_cast<ElasticContactInteraction*>(I->interactionPhysics.get())) continue;	
		#endif
		functor->go(I->interactionGeometry, I->interactionPhysics, I.get(), rootBody);
	}
}

void ef2_Spheres_Elastic_ElasticLaw::go(shared_ptr<InteractionGeometry>& ig, shared_ptr<InteractionPhysics>& ip, Interaction* contact, MetaBody* ncb){
	Real dt = Omega::instance().getTimeStep();

			int id1 = contact->getId1(), id2 = contact->getId2();
			// FIXME: mask handling should move to ConstitutiveLaw itself, outside the functors
			if( !(Body::byId(id1,ncb)->getGroupMask() & Body::byId(id2,ncb)->getGroupMask() & sdecGroupMask) ) return;
			SpheresContactGeometry*    currentContactGeometry= static_cast<SpheresContactGeometry*>(ig.get());
			ElasticContactInteraction* currentContactPhysics = static_cast<ElasticContactInteraction*>(ip.get());
			// delete interaction where spheres don't touch
			if(currentContactGeometry->penetrationDepth<0){ contact->isReal=false; return; }
	
			BodyMacroParameters* de1 				= YADE_CAST<BodyMacroParameters*>(Body::byId(id1,ncb)->physicalParameters.get());
			BodyMacroParameters* de2 				= YADE_CAST<BodyMacroParameters*>(Body::byId(id2,ncb)->physicalParameters.get());

			Vector3r& shearForce 			= currentContactPhysics->shearForce;
	
			if (contact->isNew) shearForce=Vector3r(0,0,0);
					
			Real un=currentContactGeometry->penetrationDepth;
			currentContactPhysics->normalForce=currentContactPhysics->kn*std::max(un,(Real) 0)*currentContactGeometry->normal;
	
			#ifdef SCG_SHEAR
				if(useShear){
					currentContactGeometry->updateShear(de1,de2,dt);
					shearForce=currentContactPhysics->ks*currentContactGeometry->shear;
				} else {
			#endif
					currentContactGeometry->updateShearForce(shearForce,currentContactPhysics->ks,currentContactPhysics->prevNormal,de1,de2,dt);
			#ifdef SCG_SHEAR
				}
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
			ncb->bex.addForce (id1,-f);
			ncb->bex.addForce (id2,+f);
			ncb->bex.addTorque(id1,-_c1x.Cross(f));
			ncb->bex.addTorque(id2, _c2x.Cross(f));
			currentContactPhysics->prevNormal = currentContactGeometry->normal;
}

