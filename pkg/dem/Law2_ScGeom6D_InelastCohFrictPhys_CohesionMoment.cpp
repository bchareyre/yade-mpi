/*************************************************************************
*  Copyright (C) 2012 by Ignacio Olmedo nolmedo.manich@gmail.com         *
*  Copyright (C) 2012 by François Kneib   francois.kneib@gmail.com       *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/




#include "Law2_ScGeom6D_InelastCohFrictPhys_CohesionMoment.hpp"

Real Law2_ScGeom6D_InelastCohFrictPhys_CohesionMoment::normElastEnergy()
{	//FIXME : this have to be checked and adapted
	Real normEnergy=0;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		InelastCohFrictPhys* phys = YADE_CAST<InelastCohFrictPhys*>(I->phys.get());
		if (phys) {
			normEnergy += 0.5*(phys->normalForce.squaredNorm()/phys->kn);
		}
	}
	return normEnergy;
}
Real Law2_ScGeom6D_InelastCohFrictPhys_CohesionMoment::shearElastEnergy()
{	//FIXME : this have to be checked and adapted
	Real shearEnergy=0;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		InelastCohFrictPhys* phys = YADE_CAST<InelastCohFrictPhys*>(I->phys.get());
		if (phys) {
			shearEnergy += 0.5*(phys->shearForce.squaredNorm()/phys->ks);
		}
	}
	return shearEnergy;
}



void Law2_ScGeom6D_InelastCohFrictPhys_CohesionMoment::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact)
{
//FIXME : non cohesive contact are not implemented, it would be useful to use setCohesionNow, setCohesionOnNewContacts etc ...
	const int &id1 = contact->getId1();
	const int &id2 = contact->getId2();
	const Real& dt = scene->dt;
	ScGeom6D* geom  = YADE_CAST<ScGeom6D*> (ig.get());
	InelastCohFrictPhys* phys = YADE_CAST<InelastCohFrictPhys*> (ip.get());
	if (contact->isFresh(scene)) phys->shearForce = Vector3r::Zero();
	
	Real un	= geom->penetrationDepth-phys->unp;
	Real Fn;

	State* de1 = Body::byId(id1,scene)->state.get();
	State* de2 = Body::byId(id2,scene)->state.get();
	
	
	if(un<=0){/// tension ///
		if(-un>phys->maxExten || phys->isBroken){//plastic failure.
			phys->isBroken=1;
			phys->normalForce=phys->shearForce=phys->moment_twist=phys->moment_bending=Vector3r(0,0,0);
			scene->interactions->requestErase(contact);
			return;
		}
		Fn=phys->knT*un; //elasticity
		if(-Fn>phys->maxElT || phys->onPlastT){ //so we are on plastic deformation.
			phys->onPlastT=1;
			phys->onPlastC=1; //if plasticity is reached on tension, set it to compression too.
			if(phys->maxCrpRchdT[0]<un){ //unloading/reloading on plastic deformation.
				Fn = phys->kTUnld*(un-phys->maxCrpRchdT[0])+phys->maxCrpRchdT[1];
			}
			else{//loading on plastic deformation : creep.
				Fn = -phys->maxElT+phys->kTCrp*(un+phys->maxElT/phys->knT);
				phys->maxCrpRchdT[0]=un; //new maximum is reached.
				phys->maxCrpRchdT[1]=Fn;
			}
		if (Fn>0){ //so the contact just passed the equilibrium state, set new "unp" who stores the plastic equilibrium state.
			phys->unp=geom->penetrationDepth;
			phys->maxCrpRchdT[0]=1e20;
			phys->maxElT=0;
		}
		}
		else{ //elasticity
			phys->maxCrpRchdT[0]=un;
			phys->maxCrpRchdT[1]=Fn;
		}
	}
	
	else{/// compression /// similar to tension.
		if(un>phys->maxContract || phys->isBroken){
			phys->isBroken=1;
			phys->normalForce=phys->shearForce=phys->moment_twist=phys->moment_bending=Vector3r(0,0,0);
			if(geom->penetrationDepth<=0){ //do not erase the contact while penetrationDepth<0 because it would be recreated at next timestep.
				scene->interactions->requestErase(contact);
			}
			return;
		}
		Fn=phys->knC*un;
		if(Fn>phys->maxElC || phys->onPlastC){
			phys->onPlastC=1;
			if(phys->maxCrpRchdC[0]>un){
				Fn = phys->kTUnld*(un-phys->maxCrpRchdC[0])+phys->maxCrpRchdC[1];
			}
			else{
				Fn = phys->maxElC+phys->kTCrp*(un-phys->maxElC/phys->knC);
				phys->maxCrpRchdC[0]=un;
				phys->maxCrpRchdC[1]=Fn;
			}
		if (Fn<0){
			phys->unp=geom->penetrationDepth;
			phys->maxCrpRchdC[0]=-1e20;
			phys->maxElC=0;
		}
		}
		else{
			phys->maxCrpRchdC[0]=un;
			phys->maxCrpRchdC[1]=Fn;
		}
	}

	/// Shear ///
	Vector3r shearForce = geom->rotate(phys->shearForce);
	const Vector3r& dus = geom->shearIncrement();

	//Linear elasticity giving "trial" shear force
	shearForce += phys->ks*dus;
	Real Fs = shearForce.norm();
	Real maxFs = phys->shearAdhesion;
	if (maxFs==0)maxFs = Fn*phys->tangensOfFrictionAngle;
	maxFs = std::max((Real) 0, maxFs);
	if (Fs  > maxFs) {//Plasticity condition on shear force
		if (!phys->cohesionBroken) {
			phys->cohesionBroken=1;
			phys->shearAdhesion=0;
			maxFs = max((Real) 0, Fn*phys->tangensOfFrictionAngle);
		}
		maxFs = maxFs / Fs;
		shearForce *= maxFs;
	}
	
	//rotational moment are only applied if the cohesion is not broken.
	/// Twist /// the twist law is driven by twist displacement ("getTwist()").
	if(!phys->cohesionBroken){
		Real twist = geom->getTwist() - phys->twp;
		Real twistM=twist*phys->ktw; //elastic twist moment.
		bool sgnChanged=0; //whether the twist moment just passed the equilibrium state.
		if(!contact->isFresh(scene) && phys->moment_twist.dot(twistM*geom->normal)<0)sgnChanged=1;
		if(abs(twist)>phys->maxTwist){
			phys->cohesionBroken=1;
			twistM=0;
		}
		else{
			if(abs(twistM)>phys->maxElTw || phys->onPlastTw){ //plastic deformation.
				phys->onPlastTw=1;
				if(abs(phys->maxCrpRchdTw[0])>abs(twist)){ //unloading/reloading
					twistM = phys->kTwUnld*(twist-phys->maxCrpRchdTw[0])+phys->maxCrpRchdTw[1];
				}
				else{//creep loading.
					int sign = twist<0?-1:1;
					twistM = sign*phys->maxElTw+phys->kTwCrp*(twist-sign*phys->maxElTw/phys->ktw);	//creep
					phys->maxCrpRchdTw[0]=twist; //new maximum reached
					phys->maxCrpRchdTw[1]=twistM;
				}
			if(sgnChanged){
				phys->maxElTw=0;
				phys->twp=geom->getTwist();
				phys->maxCrpRchdTw[0]=0;
			}
			}
			else{ //elasticity
				phys->maxCrpRchdTw[0]=twist;
				phys->maxCrpRchdTw[1]=twistM;
			}
		}
		phys->moment_twist = twistM * geom->normal;
	}
	else phys->moment_twist=Vector3r(0,0,0);
	
	/// Bending /// incremental form.
	if(!phys->cohesionBroken){
		Vector3r bendM = phys->moment_bending;
		Vector3r relAngVel = geom->getRelAngVel(de1,de2,dt);
		Vector3r relRotBend = (relAngVel - geom->normal.dot(relAngVel)*geom->normal)*dt; // relative rotation due to rolling behaviour
		bendM = geom->rotate(phys->moment_bending); // rotate moment vector (updated)
		phys->pureCreep=geom->rotate(phys->pureCreep); // pure creep is updated to compute the damage.
		Vector3r bendM_elast = bendM-phys->kr*relRotBend;
		if(bendM_elast.norm()>phys->maxElB || phys->onPlastB){ // plastic behavior 
			phys->onPlastB=1;
			bendM=bendM-phys->kDam*relRotBend; //trial bending
			if(bendM.norm()<phys->moment_bending.norm()){ // if bending decreased, we are unloading ...
				bendM = bendM+phys->kDam*relRotBend-phys->kRUnld*relRotBend; // ... so undo bendM and apply unload coefficient.
				Vector3r newPureCreep = phys->pureCreep-phys->kRCrp*relRotBend; // trial pure creep.
				phys->pureCreep = newPureCreep.norm()<phys->pureCreep.norm()?newPureCreep:phys->pureCreep+phys->kRCrp*relRotBend; // while unloading, pure creep must decrease.
				phys->kDam=phys->kr+(phys->kRCrp-phys->kr)*(phys->maxCrpRchdB.norm()-phys->maxElB)/(phys->maxBendMom-phys->maxElB); // compute the damage coefficient.
			}
			else{ // bending increased, so we are loading (bendM has to be unchanged).
				Vector3r newPureCreep = phys->pureCreep-phys->kRCrp*relRotBend;
				phys->pureCreep = newPureCreep.norm()>phys->pureCreep.norm()?newPureCreep:phys->pureCreep+phys->kRCrp*relRotBend; // while loading, pure creep must increase.
				if(phys->pureCreep.norm()<bendM.norm()) bendM=phys->pureCreep; // bending moment can't be greather than pure creep.
				if(phys->pureCreep.norm()>phys->maxCrpRchdB.norm()) phys->maxCrpRchdB=phys->pureCreep; // maxCrpRchdB must follow the maximum of pure creep.
				if(phys->pureCreep.norm()>phys->maxBendMom){
					phys->cohesionBroken=1;
					bendM=bendM_elast=Vector3r(0,0,0);
				}
			}
			phys->moment_bending=bendM;
		}
		else{//elasticity
			phys->pureCreep=phys->moment_bending=phys->maxCrpRchdB=bendM_elast;
			phys->kDam=phys->kRCrp;
		}
	}
	phys->shearForce=shearForce;
	phys->normalForce=-Fn*geom->normal;
	applyForceAtContactPoint(phys->normalForce+phys->shearForce, geom->contactPoint, id1, de1->se3.position, id2, de2->se3.position + (scene->isPeriodic ? scene->cell->intrShiftPos(contact->cellDist): Vector3r::Zero()));
	scene->forces.addTorque(id1,-phys->moment_bending-phys->moment_twist);
	scene->forces.addTorque(id2,phys->moment_bending+phys->moment_twist);
}

YADE_PLUGIN((Law2_ScGeom6D_InelastCohFrictPhys_CohesionMoment));







