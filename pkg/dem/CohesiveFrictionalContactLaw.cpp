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

YADE_PLUGIN((CohesiveFrictionalContactLaw)(Law2_ScGeom_CohFrictPhys_CohesionMoment));
CREATE_LOGGER(Law2_ScGeom_CohFrictPhys_CohesionMoment);

Vector3r translation_vect_ ( 0.10,0,0 );

void CohesiveFrictionalContactLaw::action()
{
	if(!functor) functor=shared_ptr<Law2_ScGeom_CohFrictPhys_CohesionMoment>(new Law2_ScGeom_CohFrictPhys_CohesionMoment);
	functor->always_use_moment_law = always_use_moment_law;
	functor->shear_creep=shear_creep;
	functor->twist_creep=twist_creep;
	functor->creep_viscosity=creep_viscosity;
	functor->scene=scene;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		functor->go(I->geom, I->phys, I.get());}
}

void Law2_ScGeom_CohFrictPhys_CohesionMoment::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact)
{
	const Real& dt = scene->dt;
	const int &id1 = contact->getId1();
	const int &id2 = contact->getId2();
	ScGeom6D* currentContactGeometry  = YADE_CAST<ScGeom6D*> (ig.get());
	CohFrictPhys* currentContactPhysics = YADE_CAST<CohFrictPhys*> (ip.get());
	Vector3r& shearForce    = currentContactPhysics->shearForce;

	if (contact->isFresh(scene)) shearForce   = Vector3r::Zero();
	Real un     = currentContactGeometry->penetrationDepth;
	Real Fn    = currentContactPhysics->kn*un;
	currentContactPhysics->normalForce = Fn*currentContactGeometry->normal;
	if (un < 0 && (currentContactPhysics->normalForce.squaredNorm() > pow(currentContactPhysics->normalAdhesion,2)
	               || currentContactPhysics->normalAdhesion==0)) {
		// BREAK due to tension
		scene->interactions->requestErase(contact->getId1(),contact->getId2());
	} else {
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

		/// Moment law        ///
		if (currentContactPhysics->momentRotationLaw && (!currentContactPhysics->cohesionBroken || always_use_moment_law)) {
			if (twist_creep) {
				Real viscosity_twist = creep_viscosity * std::pow((2 * std::min(currentContactGeometry->radius1,currentContactGeometry->radius2)),2) / 16.0;
				Real angle_twist_creeped = currentContactGeometry->getTwist() * (1 - dt/viscosity_twist);
				Quaternionr q_twist(AngleAxisr(currentContactGeometry->getTwist(),currentContactGeometry->normal));
				Quaternionr q_twist_creeped(AngleAxisr(angle_twist_creeped,currentContactGeometry->normal));
				Quaternionr q_twist_delta(q_twist_creeped * q_twist.conjugate());
				currentContactGeometry->twistCreep = currentContactGeometry->twistCreep * q_twist_delta;
			}
			currentContactPhysics->moment_twist = (currentContactGeometry->getTwist()*currentContactPhysics->kr)*currentContactGeometry->normal;
			currentContactPhysics->moment_bending = currentContactGeometry->getBending() * currentContactPhysics->kr;
			Vector3r moment = currentContactPhysics->moment_twist + currentContactPhysics->moment_bending;
			scene->forces.addTorque(id1,-moment);
			scene->forces.addTorque(id2, moment);
		}
		/// Moment law END       ///
	}
}
