/*************************************************************************
*  Copyright (C) 2005 by Bruno Chareyre   bruno.chareyre@hmg.inpg.fr     *
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

YADE_PLUGIN((Law2_ScGeom_FrictPhys_Basic)(Law2_Dem3DofGeom_FrictPhys_Basic)(ElasticContactLaw));

Real Law2_ScGeom_FrictPhys_Basic::Real0=0;
Real Law2_ScGeom_FrictPhys_Basic::getPlasticDissipation() {return (Real) plasticDissipation;}
void Law2_ScGeom_FrictPhys_Basic::initPlasticDissipation(Real initVal) {plasticDissipation.reset(); plasticDissipation+=initVal;}

void ElasticContactLaw::action()
{
	if(!functor) functor=shared_ptr<Law2_ScGeom_FrictPhys_Basic>(new Law2_ScGeom_FrictPhys_Basic);
	functor->neverErase=neverErase;
	functor->scene=scene;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		#ifdef YADE_DEBUG
			// these checks would be redundant in the functor (LawDispatcher does that already)
			if(!dynamic_cast<ScGeom*>(I->interactionGeometry.get()) || !dynamic_cast<FrictPhys*>(I->interactionPhysics.get())) continue;	
		#endif
			functor->go(I->interactionGeometry, I->interactionPhysics, I.get());
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
void Law2_ScGeom_FrictPhys_Basic::go(shared_ptr<InteractionGeometry>& ig, shared_ptr<InteractionPhysics>& ip, Interaction* contact){
	int id1 = contact->getId1(), id2 = contact->getId2();

	ScGeom*    geom= static_cast<ScGeom*>(ig.get());
	FrictPhys* phys = static_cast<FrictPhys*>(ip.get());
	if(geom->penetrationDepth <0){
		if (neverErase) {
			phys->shearForce = Vector3r::Zero();
			phys->normalForce = Vector3r::Zero();}
		else 	scene->interactions->requestErase(id1,id2);
		return;}
	State* de1 = Body::byId(id1,scene)->state.get();
	State* de2 = Body::byId(id2,scene)->state.get();
	Real& un=geom->penetrationDepth;
	TRVAR3(geom->penetrationDepth,de1->se3.position,de2->se3.position);
	phys->normalForce=phys->kn*std::max(un,(Real) 0)*geom->normal;

	Vector3r& shearForce = geom->rotate(phys->shearForce);
	const Vector3r& shearDisp = geom->shearIncrement();
	shearForce -= phys->ks*shearDisp;
	Real maxFs = phys->normalForce.squaredNorm()*std::pow(phys->tangensOfFrictionAngle,2);

	if (!traceEnergy){//Update force but don't compute energy terms (see below))
		// PFC3d SlipModel, is using friction angle. CoulombCriterion
		if( shearForce.squaredNorm() > maxFs ){
			Real ratio = Mathr::Sqrt(maxFs) / shearForce.norm();
			shearForce *= ratio;}
	} else {
		//almost the same with additional Vector3r instanciated for energy tracing, duplicated block to make sure there is no cost for the instanciation of the vector when traceEnergy==false
		if( shearForce.squaredNorm() > maxFs ){
			Real ratio = Mathr::Sqrt(maxFs) / shearForce.norm();
			Vector3r trialForce=shearForce;//store prev force for definition of plastic slip
			//define the plastic work input and increment the total plastic energy dissipated
			shearForce *= ratio;
			plasticDissipation +=
			((1/phys->ks)*(trialForce-shearForce))//plastic disp.
			.dot(shearForce);//active force
		}
	}
	if (!scene->isPeriodic)
		applyForceAtContactPoint(-phys->normalForce-shearForce, geom->contactPoint, id1, de1->se3.position, id2, de2->se3.position);
	else {//we need to use correct branches in the periodic case, the following apply for spheres only
		Vector3r force = -phys->normalForce-shearForce;
		scene->forces.addForce(id1,force);
		scene->forces.addForce(id2,-force);
		scene->forces.addTorque(id1,(geom->radius1-0.5*geom->penetrationDepth)* geom->normal.cross(force));
		scene->forces.addTorque(id2,(geom->radius2-0.5*geom->penetrationDepth)* geom->normal.cross(force));
	}
	//FIXME : make sure phys->prevNormal is not used anywhere, remove it from physics and remove this line :
	phys->prevNormal = geom->normal;
}

// same as elasticContactLaw, but using Dem3DofGeom
void Law2_Dem3DofGeom_FrictPhys_Basic::go(shared_ptr<InteractionGeometry>& ig, shared_ptr<InteractionPhysics>& ip, Interaction* contact){
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
	applyForceAtContactPoint(phys->normalForce+trialFs,geom->contactPoint,contact->getId1(),geom->se31.position,contact->getId2(),geom->se32.position);
}

