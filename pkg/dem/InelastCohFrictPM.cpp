#include "InelastCohFrictPM.hpp"

YADE_PLUGIN((InelastCohFrictMat)(InelastCohFrictPhys)(Ip2_2xInelastCohFrictMat_InelastCohFrictPhys)(Law2_ScGeom6D_InelastCohFrictPhys_CohesionMoment));


void Ip2_2xInelastCohFrictMat_InelastCohFrictPhys::go(const shared_ptr<Material>& b1    // InelastCohFrictMat
                                        , const shared_ptr<Material>& b2 // InelastCohFrictMat
                                        , const shared_ptr<Interaction>& interaction)
{
	
	InelastCohFrictMat* sdec1 = static_cast<InelastCohFrictMat*>(b1.get());
	InelastCohFrictMat* sdec2 = static_cast<InelastCohFrictMat*>(b2.get());
	ScGeom6D* geom = YADE_CAST<ScGeom6D*>(interaction->geom.get());
	
	//FIXME : non cohesive contact are not implemented, it would be useful to use setCohesionNow, setCohesionOnNewContacts etc ...

	if (geom) {
		if (!interaction->phys) {
			interaction->phys = shared_ptr<InelastCohFrictPhys>(new InelastCohFrictPhys());
			InelastCohFrictPhys* contactPhysics = YADE_CAST<InelastCohFrictPhys*>(interaction->phys.get());
			Real pi = 3.14159265;
			Real r1 	= geom->radius1;
			Real r2 	= geom->radius2;
			Real f1 	= sdec1->frictionAngle;
			Real f2 	= sdec2->frictionAngle;
			
			contactPhysics->tangensOfFrictionAngle	= tan(min(f1,f2));
			
			// harmonic average of modulus
			contactPhysics->knC = 2.0*sdec1->compressionModulus*r1*sdec2->compressionModulus*r2/(sdec1->compressionModulus*r1+sdec2->compressionModulus*r2);
			contactPhysics->knT = 2.0*sdec1->tensionModulus*r1*sdec2->tensionModulus*r2/(sdec1->tensionModulus*r1+sdec2->tensionModulus*r2);
			contactPhysics->ks = 2.0*sdec1->shearModulus*r1*sdec2->shearModulus*r2/(sdec1->shearModulus*r1+sdec2->shearModulus*r2); 
			
			// harmonic average of coeficients for bending and twist coeficients
			Real AlphaKr = 2.0*sdec1->alphaKr*sdec2->alphaKr/(sdec1->alphaKr+sdec2->alphaKr);
			Real AlphaKtw = 2.0*sdec1->alphaKtw*sdec2->alphaKtw/(sdec1->alphaKtw+sdec2->alphaKtw);
			
			contactPhysics->kr = r1*r2*contactPhysics->ks*AlphaKr;
			contactPhysics->ktw = r1*r2*contactPhysics->ks*AlphaKtw;
			
			contactPhysics->kTCrp	= contactPhysics->knT*min(sdec1->creepTension,sdec2->creepTension);
			contactPhysics->kRCrp	= contactPhysics->kr*min(sdec1->creepBending,sdec2->creepBending);
			contactPhysics->kTwCrp	= contactPhysics->ktw*min(sdec1->creepTwist,sdec2->creepTwist);
			
			contactPhysics->kRUnld =  contactPhysics->kr*min(sdec1->unloadBending,sdec2->unloadBending);
			contactPhysics->kTUnld =  contactPhysics->knT*min(sdec1->unloadTension,sdec2->unloadTension);
			contactPhysics->kTwUnld = contactPhysics->ktw*min(sdec1->unloadTwist,sdec2->unloadTwist);

			contactPhysics->maxElC =  min(sdec1->sigmaCompression,sdec2->sigmaCompression)*pow(min(r2, r1),2);
			contactPhysics->maxElT =  min(sdec1->sigmaTension,sdec2->sigmaTension)*pow(min(r2, r1),2);
			contactPhysics->maxElB =  min(sdec1->nuBending,sdec2->nuBending)*pow(min(r2, r1),3);
			contactPhysics->maxElTw = min(sdec1->nuTwist,sdec2->nuTwist)*pow(min(r2, r1),3);
								
			contactPhysics->shearAdhesion = min(sdec1->shearCohesion,sdec2->shearCohesion)*pow(min(r1, r2),2);
			
			contactPhysics->maxExten = min(sdec1->epsilonMaxTension*r1,sdec2->epsilonMaxTension*r2);
			contactPhysics->maxContract = min(sdec1->epsilonMaxCompression*r1,sdec2->epsilonMaxCompression*r2);
			
			contactPhysics->maxBendMom = min(sdec1->etaMaxBending,sdec2->etaMaxBending)*pow(min(r2, r1),3);
			contactPhysics->maxTwist = 2*pi*min(sdec1->etaMaxTwist,sdec2->etaMaxTwist);
		}
	}
};



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
		if(std::abs(twist)>phys->maxTwist){
			phys->cohesionBroken=1;
			twistM=0;
		}
		else{
			if(std::abs(twistM)>phys->maxElTw || phys->onPlastTw){ //plastic deformation.
				phys->onPlastTw=1;
				if(std::abs(phys->maxCrpRchdTw[0])>std::abs(twist)){ //unloading/reloading
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
