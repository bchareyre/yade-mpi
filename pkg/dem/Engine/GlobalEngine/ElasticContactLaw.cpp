/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi   olivier.galizzi@imag.fr       *
*  Copyright (C) 2009 by Bruno Chareyre   bruno.chareyre@hmg.inpg.fr     *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"ElasticContactLaw.hpp"
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/pkg-dem/FrictPhys.hpp>
#include<yade/pkg-dem/DemXDofGeom.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>
#include<yade/core/Scene.hpp>

YADE_PLUGIN((Law2_ScGeom_FrictPhys_Basic)(Law2_Dem3DofGeom_FrictPhys_Basic)(ElasticContactLaw)(Law2_Dem6DofGeom_FrictPhys_Beam));

Real Law2_ScGeom_FrictPhys_Basic::Real0=0;
Real Law2_ScGeom_FrictPhys_Basic::getPlasticDissipation() {return (Real) plasticDissipation;}
void Law2_ScGeom_FrictPhys_Basic::initPlasticDissipation(Real initVal) {plasticDissipation.reset(); plasticDissipation+=initVal;}

void ElasticContactLaw::action()
{
	if(!functor) functor=shared_ptr<Law2_ScGeom_FrictPhys_Basic>(new Law2_ScGeom_FrictPhys_Basic);
	functor->useShear=useShear;
	functor->neverErase=neverErase;
	functor->scene=scene;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		#ifdef YADE_DEBUG
			// these checks would be redundant in the functor (LawDispatcher does that already)
			if(!dynamic_cast<ScGeom*>(I->interactionGeometry.get()) || !dynamic_cast<FrictPhys*>(I->interactionPhysics.get())) continue;	
		#endif
			functor->go(I->interactionGeometry, I->interactionPhysics, I.get(), scene);
	}
}

Real Law2_ScGeom_FrictPhys_Basic::elasticEnergy()
{
	Real energy=0;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		FrictPhys* phys = dynamic_cast<FrictPhys*>(I->interactionPhysics.get());
		if(phys) {
			energy += 0.5*(phys->normalForce.squaredNorm()/phys->kn + phys->shearForce.squaredNorm()/phys->ks);}
	}
	return energy;
}

CREATE_LOGGER(Law2_ScGeom_FrictPhys_Basic);
void Law2_ScGeom_FrictPhys_Basic::go(shared_ptr<InteractionGeometry>& ig, shared_ptr<InteractionPhysics>& ip, Interaction* contact, Scene* ncb){
	const Real& dt = scene->dt;
	int id1 = contact->getId1(), id2 = contact->getId2();

	ScGeom*    currentContactGeometry= static_cast<ScGeom*>(ig.get());
	FrictPhys* currentContactPhysics = static_cast<FrictPhys*>(ip.get());
	if(currentContactGeometry->penetrationDepth <0){
		if (neverErase) {
			currentContactPhysics->shearForce = Vector3r::Zero();
			currentContactPhysics->normalForce = Vector3r::Zero();}
		else 	ncb->interactions->requestErase(id1,id2);
		return;}
	State* de1 = Body::byId(id1,ncb)->state.get();
	State* de2 = Body::byId(id2,ncb)->state.get();
	Vector3r& shearForce = currentContactPhysics->shearForce;
	Real un=currentContactGeometry->penetrationDepth;
	TRVAR3(currentContactGeometry->penetrationDepth,de1->se3.position,de2->se3.position);
	currentContactPhysics->normalForce=currentContactPhysics->kn*std::max(un,(Real) 0)*currentContactGeometry->normal;
	//FIXME : it would make more sense to compute the shift in some Ig2->getShear, using precomputed velGrad*Hsize in Cell and I->cellDist, but it is not possible with current design (Ig doesn't know scene nor I).
	Vector3r shiftVel = scene->isPeriodic ? (Vector3r)((scene->cell->velGrad*scene->cell->Hsize)*Vector3r((Real) contact->cellDist[0],(Real) contact->cellDist[1],(Real) contact->cellDist[2])) : Vector3r::Zero();
	if (!traceEnergy){//Update force but don't compute energy terms (see below))
		if(useShear){
			currentContactGeometry->updateShear(de1,de2,dt);
			shearForce=currentContactPhysics->ks*currentContactGeometry->shear;
		} else {
			Vector3r dus = currentContactGeometry->rotateAndGetShear(shearForce,currentContactPhysics->prevNormal,de1,de2,dt,shiftVel,/*avoid ratcheting*/true);
			//Linear elasticity giving "trial" shear force
			shearForce -= currentContactPhysics->ks*dus;
		}
		// PFC3d SlipModel, is using friction angle. CoulombCriterion
		Real maxFs = currentContactPhysics->normalForce.squaredNorm()*
			std::pow(currentContactPhysics->tangensOfFrictionAngle,2);
		if( shearForce.squaredNorm() > maxFs ){
			Real ratio = Mathr::Sqrt(maxFs) / shearForce.norm();
			shearForce *= ratio;
			if(useShear) currentContactGeometry->shear*=ratio;}
	} else {
		//almost the same with 2 additional Vector3r instanciated for energy tracing, duplicated block to make sure there is no cost for the instanciation of the vectors when traceEnergy==false
		if(useShear) throw ("energy tracing not defined with useShear==true");
		Vector3r shearDisp = currentContactGeometry->rotateAndGetShear(shearForce,currentContactPhysics->prevNormal,de1,de2,dt,shiftVel,true);
		Vector3r prevForce=shearForce;//store prev force for definition of plastic slip
		shearForce -= currentContactPhysics->ks*shearDisp;
		Real maxFs = currentContactPhysics->normalForce.squaredNorm()*
			std::pow(currentContactPhysics->tangensOfFrictionAngle,2);
		if( shearForce.squaredNorm() > maxFs ){
			Real ratio = Mathr::Sqrt(maxFs) / shearForce.norm();
			Vector3r trialForce=shearForce;//store prev force for definition of plastic slip
			//define the plastic work input and increment the total plastic energy dissipated
			shearForce *= ratio;
			plasticDissipation +=
			((1/currentContactPhysics->ks)*(trialForce-shearForce))//plastic disp.
			.dot(shearForce);//active force
		}
	}
	if (!scene->isPeriodic)
	applyForceAtContactPoint(-currentContactPhysics->normalForce-shearForce, currentContactGeometry->contactPoint, id1, de1->se3.position, id2, de2->se3.position, ncb);
	else {//we need to use correct branches in the periodic case, the following apply for spheres only
		Vector3r force = -currentContactPhysics->normalForce-shearForce;
		ncb->forces.addForce(id1,force);
		ncb->forces.addForce(id2,-force);
		ncb->forces.addTorque(id1,(currentContactGeometry->radius1-0.5*currentContactGeometry->penetrationDepth)* currentContactGeometry->normal.cross(force));
		ncb->forces.addTorque(id2,(currentContactGeometry->radius2-0.5*currentContactGeometry->penetrationDepth)* currentContactGeometry->normal.cross(force));
	}
	currentContactPhysics->prevNormal = currentContactGeometry->normal;
}

// same as elasticContactLaw, but using Dem3DofGeom
void Law2_Dem3DofGeom_FrictPhys_Basic::go(shared_ptr<InteractionGeometry>& ig, shared_ptr<InteractionPhysics>& ip, Interaction* contact, Scene*){
	Dem3DofGeom* geom=static_cast<Dem3DofGeom*>(ig.get());
	FrictPhys* phys=static_cast<FrictPhys*>(ip.get());
	Real displN=geom->displacementN();
	if(displN>0){ scene->interactions->requestErase(contact->getId1(),contact->getId2()); return; }
	phys->normalForce=phys->kn*displN*geom->normal;
	Real maxFsSq=phys->normalForce.squaredNorm()*pow(phys->tangensOfFrictionAngle,2);
	Vector3r trialFs=phys->ks*geom->displacementT();

//	FIXME : This is generating bug https://bugs.launchpad.net/bugs/585898
//  	Real trialFsSq = trialFs.squaredNorm();
//	if(trialFsSq>maxFsSq){
// 		Real multiplier=sqrt(maxFsSq/trialFsSq);
// 		geom->scaleDisplacementT(multiplier); trialFs*=multiplier;}
	//Workaround start
	if(trialFs.squaredNorm()>maxFsSq){ geom->slipToDisplacementTMax(sqrt(maxFsSq)/phys->ks); trialFs*=sqrt(maxFsSq/(trialFs.squaredNorm()));}
	//Workaround end
	phys->shearForce=trialFs;
	applyForceAtContactPoint(phys->normalForce+trialFs,geom->contactPoint,contact->getId1(),geom->se31.position,contact->getId2(),geom->se32.position,scene);
}

// same as elasticContactLaw, but using Dem3DofGeom
void Law2_Dem6DofGeom_FrictPhys_Beam::go(shared_ptr<InteractionGeometry>& ig, shared_ptr<InteractionPhysics>& ip, Interaction* contact, Scene* scene){
	// normal & shear forces
	Dem6DofGeom* geom=static_cast<Dem6DofGeom*>(ig.get());
	FrictPhys* phys=static_cast<FrictPhys*>(ip.get());
	Real displN=geom->displacementN();
	phys->normalForce=phys->kn*displN*geom->normal;
	phys->shearForce=phys->ks*geom->displacementT();
	applyForceAtContactPoint(phys->normalForce+phys->shearForce,geom->contactPoint,contact->getId1(),geom->se31.position,contact->getId2(),geom->se32.position,scene);
	// bend&twist:
	Vector3r bend; Real twist;
	geom->bendTwistAbs(bend,twist);
	Vector3r tt=bend*phys->kn+geom->normal*twist*phys->kn;
	cerr<<twist<<";"<<bend<<endl;
	scene->forces.addTorque(contact->getId1(),tt);
	scene->forces.addTorque(contact->getId2(),-tt);
}
