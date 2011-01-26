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

Vector3r translation_vect_ ( 0.10,0,0 );

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
	ScGeom6D* currentContactGeometry  = YADE_CAST<ScGeom6D*> (ig.get());
	CohFrictPhys* currentContactPhysics = YADE_CAST<CohFrictPhys*> (ip.get());
	Vector3r& shearForce    = currentContactPhysics->shearForce;

	if (contact->isFresh(scene)) shearForce   = Vector3r::Zero();
	Real un     = currentContactGeometry->penetrationDepth;
	Real Fn    = currentContactPhysics->kn*(un-currentContactPhysics->unp);

	if (currentContactPhysics->fragile && (-Fn)> currentContactPhysics->normalAdhesion) {
		// BREAK due to tension
		scene->interactions->requestErase(contact->getId1(),contact->getId2());
	} else {
		if ((-Fn)> currentContactPhysics->normalAdhesion) {//normal plasticity
			Fn=-currentContactPhysics->normalAdhesion;
			currentContactPhysics->unp = un+currentContactPhysics->normalAdhesion/currentContactPhysics->kn;
			if (currentContactPhysics->unpMax && currentContactPhysics->unp<currentContactPhysics->unpMax)
				scene->interactions->requestErase(contact->getId1(),contact->getId2());
		}
		currentContactPhysics->normalForce = Fn*currentContactGeometry->normal;
		State* de1 = Body::byId(id1,scene)->state.get();
		State* de2 = Body::byId(id2,scene)->state.get();
		///////////////////////// CREEP START ///////////
		if (shear_creep) shearForce -= currentContactPhysics->ks*(shearForce*dt/creep_viscosity);
		///////////////////////// CREEP END ////////////

		Vector3r& shearForce = currentContactGeometry->rotate(currentContactPhysics->shearForce);
		const Vector3r& dus = currentContactGeometry->shearIncrement();

		//Linear elasticity giving "trial" shear force
		shearForce -= currentContactPhysics->ks*dus;

		Real Fs = currentContactPhysics->shearForce.norm();
		Real maxFs = currentContactPhysics->shearAdhesion;
		if (!currentContactPhysics->cohesionDisablesFriction || maxFs==0)
			maxFs += Fn*currentContactPhysics->tangensOfFrictionAngle;
		maxFs = std::max((Real) 0, maxFs);
		if (Fs  > maxFs) {//Plasticity condition on shear force
			if (currentContactPhysics->fragile && !currentContactPhysics->cohesionBroken) {
				currentContactPhysics->SetBreakingState();
				maxFs = max((Real) 0, Fn*currentContactPhysics->tangensOfFrictionAngle);
			}
			maxFs = maxFs / Fs;
			shearForce *= maxFs;
			if (Fn<0)  currentContactPhysics->normalForce = Vector3r::Zero();//Vector3r::Zero()
		}
		applyForceAtContactPoint(-currentContactPhysics->normalForce-shearForce, currentContactGeometry->contactPoint, id1, de1->se3.position, id2, de2->se3.position);

		/// Moment law  ///
		if (currentContactPhysics->momentRotationLaw && (!currentContactPhysics->cohesionBroken || always_use_moment_law)) {
			if (!useIncrementalForm){
				if (twist_creep) {
					Real viscosity_twist = creep_viscosity * std::pow((2 * std::min(currentContactGeometry->radius1,currentContactGeometry->radius2)),2) / 16.0;
					Real angle_twist_creeped = currentContactGeometry->getTwist() * (1 - dt/viscosity_twist);
					Quaternionr q_twist(AngleAxisr(currentContactGeometry->getTwist(),currentContactGeometry->normal));
					Quaternionr q_twist_creeped(AngleAxisr(angle_twist_creeped,currentContactGeometry->normal));
					Quaternionr q_twist_delta(q_twist_creeped * q_twist.conjugate());
					currentContactGeometry->twistCreep = currentContactGeometry->twistCreep * q_twist_delta;
				}
				currentContactPhysics->moment_twist = (currentContactGeometry->getTwist()*currentContactPhysics->ktw)*currentContactGeometry->normal;
				currentContactPhysics->moment_bending = currentContactGeometry->getBending() * currentContactPhysics->kr;
			}	
			else{ // Use incremental formulation to compute moment_twis and moment_bending (no twist_creep is applied)
				if (twist_creep) throw std::invalid_argument("Law2_ScGeom6D_CohFrictPhys_CohesionMoment: no twis creep is included if the incremental form for the rotations is used.");
				Vector3r relAngVel = currentContactGeometry->getRelAngVel(de1,de2,dt);
				// *** Bending ***//
				Vector3r relAngVelBend = relAngVel - currentContactGeometry->normal.dot(relAngVel)*currentContactGeometry->normal; // keep only the bending part 
				Vector3r relRotBend = relAngVelBend*dt; // relative rotation due to rolling behaviour	
				// incremental formulation for the bending moment (as for the shear part)
				Vector3r& momentBend = currentContactPhysics->moment_bending;
				momentBend = currentContactGeometry->rotate(momentBend); // rotate moment vector (updated)
				momentBend = momentBend-currentContactPhysics->kr*relRotBend;
				// ----------------------------------------------------------------------------------------
				// *** Torsion ***//
				Vector3r relAngVelTwist = currentContactGeometry->normal.dot(relAngVel)*currentContactGeometry->normal;
				Vector3r relRotTwist = relAngVelTwist*dt; // component of relative rotation along n  FIXME: sign?
				// incremental formulation for the torsional moment
				Vector3r& momentTwist = currentContactPhysics->moment_twist;
				momentTwist = currentContactGeometry->rotate(momentTwist); // rotate moment vector (updated)
				momentTwist = momentTwist-currentContactPhysics->ktw*relRotTwist; // FIXME: sign?
			}
			/// Plasticity ///
			// limit rolling moment to the plastic value, if required
			Real RollMax = currentContactPhysics->maxRollPl*currentContactPhysics->normalForce.norm();
			if (RollMax>0.){ // do we want to apply plasticity?
				LOG_WARN("If :yref:`CohesiveFrictionalContactLaw::useIncrementalForm` is false, then plasticity would not be applied correctly (the total formulation would not reproduce irreversibility).");
				Real scalarRoll = currentContactPhysics->moment_bending.norm();		
				if (scalarRoll>RollMax){ // fix maximum rolling moment
					Real ratio = RollMax/scalarRoll;
					currentContactPhysics->moment_bending *= ratio;	
				}	
			}
			// limit twisting moment to the plastic value, if required
			Real TwistMax = currentContactPhysics->maxTwistMoment.norm();
			if (TwistMax>0.){ // do we want to apply plasticity?
				LOG_WARN("If :yref:`CohesiveFrictionalContactLaw::useIncrementalForm` is false, then plasticity would not be applied correctly (the total formulation would not reproduce irreversibility).");
				Real scalarTwist= currentContactPhysics->moment_twist.norm();		
				if (scalarTwist>TwistMax){ // fix maximum rolling moment
					Real ratio = TwistMax/scalarTwist;
					currentContactPhysics->moment_twist *= ratio;	
				}	
			}
			// Apply moments now
			Vector3r moment = currentContactPhysics->moment_twist + currentContactPhysics->moment_bending;
			scene->forces.addTorque(id1,-moment);
			scene->forces.addTorque(id2, moment);			
		}
		/// Moment law END       ///
	}
}
