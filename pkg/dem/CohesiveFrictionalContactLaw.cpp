/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre <bruno.chareyre@imag.fr>         *
*  Copyright (C) 2008 by Janek Kozicki <cosurgi@berlios.de>              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "CohesiveFrictionalContactLaw.hpp"
#include<yade/pkg/dem/CohFrictMat.hpp>
#include<yade/pkg/dem/ScGeom.hpp>
#include<yade/pkg/dem/CohFrictPhys.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>

YADE_PLUGIN((CohesiveFrictionalContactLaw)(Law2_ScGeom6D_CohFrictPhys_CohesionMoment));
CREATE_LOGGER(Law2_ScGeom6D_CohFrictPhys_CohesionMoment);

Real Law2_ScGeom6D_CohFrictPhys_CohesionMoment::getPlasticDissipation() {return (Real) plasticDissipation;}
void Law2_ScGeom6D_CohFrictPhys_CohesionMoment::initPlasticDissipation(Real initVal) {plasticDissipation.reset(); plasticDissipation+=initVal;}

Real Law2_ScGeom6D_CohFrictPhys_CohesionMoment::normElastEnergy()
{
	Real normEnergy=0;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		CohFrictPhys* phys = YADE_CAST<CohFrictPhys*>(I->phys.get());
		if (phys) {
			normEnergy += 0.5*(phys->normalForce.squaredNorm()/phys->kn);
		}
	}
	return normEnergy;
}
Real Law2_ScGeom6D_CohFrictPhys_CohesionMoment::shearElastEnergy()
{
	Real shearEnergy=0;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		CohFrictPhys* phys = YADE_CAST<CohFrictPhys*>(I->phys.get());
		if (phys) {
			shearEnergy += 0.5*(phys->shearForce.squaredNorm()/phys->ks);
		}
	}
	return shearEnergy;
}

Real Law2_ScGeom6D_CohFrictPhys_CohesionMoment::bendingElastEnergy()
{
	Real bendingEnergy=0;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		CohFrictPhys* phys = YADE_CAST<CohFrictPhys*>(I->phys.get());
		if (phys) {
			bendingEnergy += 0.5*(phys->moment_bending.squaredNorm()/phys->kr);
		}
	}
	return bendingEnergy;
}

Real Law2_ScGeom6D_CohFrictPhys_CohesionMoment::twistElastEnergy()
{
	Real twistEnergy=0;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		CohFrictPhys* phys = YADE_CAST<CohFrictPhys*>(I->phys.get());
		if (phys) {
			twistEnergy += 0.5*(phys->moment_twist.squaredNorm()/phys->ktw);
		}
	}
	return twistEnergy;
}

Real Law2_ScGeom6D_CohFrictPhys_CohesionMoment::totalElastEnergy()
{
	Real totalEnergy=0;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		CohFrictPhys* phys = YADE_CAST<CohFrictPhys*>(I->phys.get());
		if (phys) {
			totalEnergy += 0.5*(phys->normalForce.squaredNorm()/phys->kn);
			totalEnergy += 0.5*(phys->shearForce.squaredNorm()/phys->ks);
			totalEnergy += 0.5*(phys->moment_bending.squaredNorm()/phys->kr);
			totalEnergy += 0.5*(phys->moment_twist.squaredNorm()/phys->ktw);
		}
	}
	return totalEnergy;
}



void CohesiveFrictionalContactLaw::action()
{
	if(!functor) functor=shared_ptr<Law2_ScGeom6D_CohFrictPhys_CohesionMoment>(new Law2_ScGeom6D_CohFrictPhys_CohesionMoment);
	functor->always_use_moment_law = always_use_moment_law;
	functor->shear_creep=shear_creep;
	functor->twist_creep=twist_creep;
	functor->creep_viscosity=creep_viscosity;
	functor->scene=scene;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		functor->go(I->geom, I->phys, I.get());}
}

void Law2_ScGeom6D_CohFrictPhys_CohesionMoment::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact)
{
	const Real& dt = scene->dt;
	const int &id1 = contact->getId1();
	const int &id2 = contact->getId2();
	ScGeom6D* geom  = YADE_CAST<ScGeom6D*> (ig.get());
	CohFrictPhys* phys = YADE_CAST<CohFrictPhys*> (ip.get());
	Vector3r& shearForce    = phys->shearForce;

	if (contact->isFresh(scene)) shearForce   = Vector3r::Zero();
	Real un     = geom->penetrationDepth;
	Real Fn    = phys->kn*(un-phys->unp);

	if (phys->fragile && (-Fn)> phys->normalAdhesion) {
		// BREAK due to tension
		scene->interactions->requestErase(contact); return;
	} else {
		if ((-Fn)> phys->normalAdhesion) {//normal plasticity
			Fn=-phys->normalAdhesion;
			phys->unp = un+phys->normalAdhesion/phys->kn;
			if (phys->unpMax && phys->unp<phys->unpMax)
				scene->interactions->requestErase(contact); return;
		}
		phys->normalForce = Fn*geom->normal;
		State* de1 = Body::byId(id1,scene)->state.get();
		State* de2 = Body::byId(id2,scene)->state.get();
		///////////////////////// CREEP START ///////////
		if (shear_creep) shearForce -= phys->ks*(shearForce*dt/creep_viscosity);
		///////////////////////// CREEP END ////////////

		Vector3r& shearForce = geom->rotate(phys->shearForce);
		const Vector3r& dus = geom->shearIncrement();

		//Linear elasticity giving "trial" shear force
		shearForce -= phys->ks*dus;

		Real Fs = phys->shearForce.norm();
		Real maxFs = phys->shearAdhesion;
		if (!phys->cohesionDisablesFriction || maxFs==0)
			maxFs += Fn*phys->tangensOfFrictionAngle;
		maxFs = std::max((Real) 0, maxFs);
		if (Fs  > maxFs) {//Plasticity condition on shear force
			if (phys->fragile && !phys->cohesionBroken) {
				phys->SetBreakingState();
				maxFs = max((Real) 0, Fn*phys->tangensOfFrictionAngle);
			}
			maxFs = maxFs / Fs;
			Vector3r trialForce=shearForce;
			shearForce *= maxFs;
			if (scene->trackEnergy){
				Real sheardissip=((1/phys->ks)*(trialForce-shearForce))/*plastic disp*/ .dot(shearForce)/*active force*/;
				if(sheardissip>0) scene->energy->add(sheardissip,"shearDissip",shearDissipIx,/*reset*/false);}
			if (Fn<0)  phys->normalForce = Vector3r::Zero();//Vector3r::Zero()
		}
		//Apply the force
		applyForceAtContactPoint(-phys->normalForce-shearForce, geom->contactPoint, id1, de1->se3.position, id2, de2->se3.position + (scene->isPeriodic ? scene->cell->intrShiftPos(contact->cellDist): Vector3r::Zero()));

		/// Moment law  ///
		if (phys->momentRotationLaw && (!phys->cohesionBroken || always_use_moment_law)) {
			if (!useIncrementalForm){
				if (twist_creep) {
					Real viscosity_twist = creep_viscosity * std::pow((2 * std::min(geom->radius1,geom->radius2)),2) / 16.0;
					Real angle_twist_creeped = geom->getTwist() * (1 - dt/viscosity_twist);
					Quaternionr q_twist(AngleAxisr(geom->getTwist(),geom->normal));
					Quaternionr q_twist_creeped(AngleAxisr(angle_twist_creeped,geom->normal));
					Quaternionr q_twist_delta(q_twist_creeped * q_twist.conjugate());
					geom->twistCreep = geom->twistCreep * q_twist_delta;
				}
				phys->moment_twist = (geom->getTwist()*phys->ktw)*geom->normal;
				phys->moment_bending = geom->getBending() * phys->kr;
			}	
			else{ // Use incremental formulation to compute moment_twis and moment_bending (no twist_creep is applied)
				if (twist_creep) throw std::invalid_argument("Law2_ScGeom6D_CohFrictPhys_CohesionMoment: no twis creep is included if the incremental form for the rotations is used.");
				Vector3r relAngVel = geom->getRelAngVel(de1,de2,dt);
				// *** Bending ***//
				Vector3r relAngVelBend = relAngVel - geom->normal.dot(relAngVel)*geom->normal; // keep only the bending part
				Vector3r relRotBend = relAngVelBend*dt; // relative rotation due to rolling behaviour	
				// incremental formulation for the bending moment (as for the shear part)
				Vector3r& momentBend = phys->moment_bending;
				momentBend = geom->rotate(momentBend); // rotate moment vector (updated)
				momentBend = momentBend-phys->kr*relRotBend;
				// ----------------------------------------------------------------------------------------
				// *** Torsion ***//
				Vector3r relAngVelTwist = geom->normal.dot(relAngVel)*geom->normal;
				Vector3r relRotTwist = relAngVelTwist*dt; // component of relative rotation along n  FIXME: sign?
				// incremental formulation for the torsional moment
				Vector3r& momentTwist = phys->moment_twist;
				momentTwist = geom->rotate(momentTwist); // rotate moment vector (updated)
				momentTwist = momentTwist-phys->ktw*relRotTwist; // FIXME: sign?
			}
			/// Plasticity ///
			// limit rolling moment to the plastic value, if required
			Real RollMax = phys->maxRollPl*phys->normalForce.norm();
			if (RollMax>=0.){ // do we want to apply plasticity?
				if (!useIncrementalForm) LOG_WARN("If :yref:`Law2_ScGeom6D_CohFrictPhys_CohesionMoment::useIncrementalForm` is false, then plasticity will not be applied correctly (the total formulation would not reproduce irreversibility).");
				Real scalarRoll = phys->moment_bending.norm();
				if (scalarRoll>RollMax){ // fix maximum rolling moment
					Real ratio = RollMax/scalarRoll;
					phys->moment_bending *= ratio;
					if (scene->trackEnergy){
						Real bendingdissip=((1/phys->kr)*(scalarRoll-RollMax)*RollMax)/*active force*/;
						if(bendingdissip>0) scene->energy->add(bendingdissip,"bendingDissip",bendingDissipIx,/*reset*/false);}
				}
			}
			// limit twisting moment to the plastic value, if required
			Real TwistMax = phys->maxTwistPl*phys->normalForce.norm();
			if (TwistMax>=0.){ // do we want to apply plasticity?
				if (!useIncrementalForm) LOG_WARN("If :yref:`Law2_ScGeom6D_CohFrictPhys_CohesionMoment::useIncrementalForm` is false, then plasticity will not be applied correctly (the total formulation would not reproduce irreversibility).");
				Real scalarTwist= phys->moment_twist.norm();
				if (scalarTwist>TwistMax){ // fix maximum rolling moment
					Real ratio = TwistMax/scalarTwist;
					phys->moment_twist *= ratio;
					if (scene->trackEnergy){
						Real twistdissip=((1/phys->ktw)*(scalarTwist-TwistMax)*TwistMax)/*active force*/;
						if(twistdissip>0) scene->energy->add(twistdissip,"twistDissip",twistDissipIx,/*reset*/false);}
				}	
			}
			// Apply moments now
			Vector3r moment = phys->moment_twist + phys->moment_bending;
			scene->forces.addTorque(id1,-moment);
			scene->forces.addTorque(id2, moment);			
		}
		/// Moment law END       ///
	}
}
