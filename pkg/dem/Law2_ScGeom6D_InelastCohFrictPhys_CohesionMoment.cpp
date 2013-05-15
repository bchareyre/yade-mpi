#include "Law2_ScGeom6D_InelastCohFrictPhys_CohesionMoment.hpp"

Real Law2_ScGeom6D_InelastCohFrictPhys_CohesionMoment::normElastEnergy()
{
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
{
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
// 	cout<<"Law2_ScGeom6D_InelastCohFrictPhys_CohesionMoment"<<endl;
	const Real& dt = scene->dt;
	const int &id1 = contact->getId1();
	const int &id2 = contact->getId2();
	ScGeom6D* geom  = YADE_CAST<ScGeom6D*> (ig.get());
	InelastCohFrictPhys* phys = YADE_CAST<InelastCohFrictPhys*> (ip.get());
	Vector3r& shearForce    = phys->shearForce;
			
	cout<<"id1= "<<id1<<" id2= "<<id2<<endl;
	
	if (contact->isFresh(scene)) shearForce   = Vector3r::Zero();
		
	///Tension-Compresion///
	
	Real un     = geom->penetrationDepth;
	Real Fn = phys->knT*(un);
	//FIXME: Check signs on TESTS
	State* de1 = Body::byId(id1,scene)->state.get();
	State* de2 = Body::byId(id2,scene)->state.get();
	
	
	if(!(phys->isBrokenT)){
		
		/// Tension ///
		if(un<=0){
			if((un>= -(phys->dElT)) && !onplastT){
				Fn = phys->knT*(un);
// 				cout<<"TENSION ELASTIC Fn= "<<Fn<<" un= "<<un<<endl;
			}
			if(un< -(phys->dElT) || onplastT){
				onplastT = true;
				Fn = phys->knT*(-phys->dElT) + phys->crpT*(un+phys->dElT);
// 				cout<<"TENSION PLASTIC Fn= "<<Fn<<" un= "<<un<<" normalF= "<<phys->normalForce.norm()<<endl;
				
				if(phys->unloadedT ||(-phys->normalForce.norm()<=Fn)){
					Fn = phys->knT*(-phys->dElT)+ phys->crpT*(lastPlastUn+phys->dElT) + phys->unldT*(un-lastPlastUn);
					phys->unloadedT = true; 
// 					cout<<"TENSION PLASTIC unloading Fn= "<<Fn<<" un= "<<un<<endl;
					
					if(un<=lastPlastUn){ // Recovers creep after unload and reload
						phys->unloadedT=false;
						Fn = phys->knT*(-phys->dElT) + phys->crpT*(un+phys->dElT);
// 						cout<<"TENSION PLASTIC load aftr unld Fn= "<<Fn<<" un= "<<un<<" LPun= "<<lastPlastUn<<endl;
					}
					
				} // Unloading::  //FIXME: ?? Check Fn Sign
				else{// loading, applying Creeping
					lastPlastUn = un;
// 					cout<<"TENSION PLASTIC Creep Fn= "<<Fn<<" un= "<<un<<endl;
					
					if (un<-phys->epsMaxT){ // Plastic rupture //
						Fn = 0.0;
						phys->isBrokenT = true;  
// 						cout<<"TENSION PLASTIC creep BROKEN Fn= "<<Fn<<" un= "<<un<<endl;
					}
				}  
			}
		}
		/// Compresion ///
		if(un>0){
			if((un<=phys->dElC) && !onplastT){
				Fn = phys->knC*(un);
// 				cout<<"COMPRESION ELASTIC Fn= "<<Fn<<" un= "<<un<<endl;

			}
			if(un>phys->dElC || onplastT){
				onplastT = true;
				Fn = phys->knC*(phys->dElC) + phys->crpT*(un-phys->dElC);
				if(phys->unloadedC || (phys->normalForce.norm()>=Fn)){
					Fn = phys->knC*(phys->dElC)+ phys->crpT*(lastPlastUn-phys->dElC) + phys->unldT*(un-lastPlastUn);
					phys->unloadedC = true; 
// 					cout<<"COMPRESION PLASTIC unloading Fn= "<<Fn<<" un= "<<un<<endl;
					
					if(un>=lastPlastUn){ // Recovers creep after unload and reload
						phys->unloadedC=false;
						Fn = phys->knC*(phys->dElC) + phys->crpT*(un-phys->dElC);
// 						cout<<"COMPRESION PLASTIC load aftr unld Fn= "<<Fn<<" un= "<<un<<" LPun= "<<lastPlastUn<<endl;
					}
				
				} // Unloading::  //FIXME: Verify Fn Sign
				else{// loading, applying Creeping
					lastPlastUn = un;
// 					cout<<"COMPRESION PLASTIC Creep Fn= "<<Fn<<" un= "<<un<<endl;
					// Fn stills Fn = phys->crpT*(un-phys->unp) ;}
					if (un>phys->epsMaxC){ // Plastic rupture //
						Fn = 0.0;
						phys->isBrokenT = true;  
// 						cout<<"COMPRESION PLASTIC creep BROKEN Fn= "<<Fn<<" un= "<<un<<endl;
					}
				}  
			}
		}
	phys->normalForce = Fn*geom->normal;
	}
// 	if ((-Fn)> phys->normalAdhesion) {//normal plasticity
// 		Fn=-phys->normalAdhesion;
// 		phys->unp = un+phys->normalAdhesion/phys->kn;
// 		if (phys->unpMax && phys->unp<phys->unpMax)
// 			scene->interactions->requestErase(contact); return;
// 	}
	
	// 	cout<<"Tension-Comp normalF= "<<phys->normalForce<<endl;
	///end tension-compression///
	
	///Shear/// ELASTOPLASTIC perfect law TO BE DONE
	//FIXME:: TO DO::Shear ElastoPlastic perfect LAW  
	///////////////////////// CREEP START ///////////
	if (shear_creep) shearForce -= phys->ks*(shearForce*dt/creep_viscosity);
	///////////////////////// CREEP END ////////////

	shearForce = geom->rotate(phys->shearForce);
	const Vector3r& dus = geom->shearIncrement();

	//Linear elasticity giving "trial" shear force
	shearForce -= phys->ks*dus;
	
	
	Real Fs = phys->shearForce.norm();
	Real maxFs = phys->shearAdhesion;
	
	
	if (!phys->cohesionDisablesFriction || maxFs==0)
		maxFs += Fn*phys->tangensOfFrictionAngle;
		maxFs = std::max((Real) 0, maxFs);
	if (Fs  > maxFs) {//Plasticity condition on shear force
// 		cout<<"Plastshear ShearAdh= "<<phys->shearAdhesion<<endl;
		if (phys->fragile && !phys->cohesionBroken) {
			phys->SetBreakingState();
			maxFs = max((Real) 0, Fn*phys->tangensOfFrictionAngle);
		}
		maxFs = maxFs / Fs;
		Vector3r trialForce=shearForce;
		shearForce *= maxFs;
		if (scene->trackEnergy){
			Real dissip=((1/phys->ks)*(trialForce-shearForce))/*plastic disp*/ .dot(shearForce)/*active force*/;
			if(dissip>0) scene->energy->add(dissip,"plastDissip",plastDissipIx,/*reset*/false);}
		if (Fn<0)  phys->normalForce = Vector3r::Zero();//Vector3r::Zero()
	}
// 	cout<<"Fs= "<<Fs<<" maxFs= "<<maxFs<<endl;

	//Apply the force
	applyForceAtContactPoint(-phys->normalForce-shearForce, geom->contactPoint, id1, de1->se3.position, id2, de2->se3.position + (scene->isPeriodic ? scene->cell->intrShiftPos(contact->cellDist): Vector3r::Zero()));
// 		Vector3r force = -phys->normalForce-shearForce;
// 		scene->forces.addForce(id1,force);
// 		scene->forces.addForce(id2,-force);
// 		scene->forces.addTorque(id1,(geom->radius1-0.5*geom->penetrationDepth)* geom->normal.cross(force));
// 		scene->forces.addTorque(id2,(geom->radius2-0.5*geom->penetrationDepth)* geom->normal.cross(force));
	/// end Shear ///
	
	/// Moment law  ///
	/// Bending///
	if(!(phys->isBrokenB)){
		Vector3r relAngVel = geom->getRelAngVel(de1,de2,dt);
		Vector3r relAngVelBend = relAngVel - geom->normal.dot(relAngVel)*geom->normal; // keep only the bending part
		Vector3r relRotBend = relAngVelBend*dt; // relative rotation due to rolling behaviour	
		Vector3r& momentBend = phys->moment_bending;
		momentBend = geom->rotate(momentBend); // rotate moment vector (updated)

		//To check if in elastic zone in current iteration
		Real BendValue = (phys->moment_bending-phys->kr*relRotBend).norm();
		Real MaxElastB = phys->maxElastB;
		bool elasticBState = (BendValue<=MaxElastB);
		
		if(!onplastB && elasticBState){
			momentBend = momentBend-phys->kr*relRotBend;
// 			cout<<"BENDING Elastic"<<" momentB= "<<momentBend<<endl;
		
		}else{  ///Bending Plasticity///
			onplastB = true;
			BendValue = (phys->moment_bending-phys->crpB*relRotBend).norm();
// 			cout<<"BENDING Plastic"<<" momentB= "<<momentBend<<endl;
			// Unloading:: RelRotBend > 0 ::::
			if(phys->unloadedB || phys->moment_bending.norm()>=BendValue){
				momentBend = momentBend-phys->unldB*relRotBend;
				phys->unloadedB = true; 
// 				cout<<"BENDING Plastic UNLD"<<" momentB= "<<momentBend<<endl;
				if(BendValue>=lastPlastBend){phys->unloadedB = false;} 
				
			} 
			else{
				momentBend = momentBend-phys->crpB*relRotBend;// loading, applying Creeping
				Vector3r AbsRot = de1->rot()-de2->rot();
				Real AbsBending = (AbsRot - geom->normal.dot(AbsRot)*geom->normal).norm();
				lastPlastBend= BendValue;
				if (AbsBending>phys->phBMax){ // Plastic rupture //FIXME: This line is not ok, need to find the compare the total(or just plastic) angular displacement to PhBmax, true meaning of relRotBend??
					momentBend = Vector3r(0,0,0);
					phys->isBrokenB = true;
// 					cout<<"BENDING Plastic BREAK"<<" momentB= "<<momentBend<<endl;
				}
			}	
		}
		phys->moment_bending = momentBend;
	}	
	
	///Twist///
	
	if(!(phys->isBrokenTw)){
		Vector3r relAngVel = geom->getRelAngVel(de1,de2,dt);
		Vector3r relAngVelTwist = geom->normal.dot(relAngVel)*geom->normal;
		Vector3r relRotTwist = relAngVelTwist*dt; // component of relative rotation along n  FIXME: sign?
		Vector3r& momentTwist = phys->moment_twist;
		momentTwist = geom->rotate(momentTwist); // rotate moment vector (updated)
		
		//To check if in elastic zone in current iteration
		Real TwistValue = (phys->moment_twist-phys->kt*relRotTwist).norm();
		Real MaxElastTw = phys->maxElastTw;
		bool elasticTwState = (TwistValue<=MaxElastTw);
		
		if (!onplastTw && elasticTwState){
			momentTwist = momentTwist-phys->kt*relRotTwist; // FIXME: sign?
// 			cout<<"TWIST Elast"<<" momentTwist="<<momentTwist<<endl;
		}else {  ///Twist Plasticity///
			onplastTw = true;
			TwistValue = (phys->moment_twist-phys->crpTw*relRotTwist).norm();
// 			cout<<"TWIST Plast"<<endl;
			if (phys->unloadedTw || phys->moment_twist.norm()>=TwistValue){// Unloading:: RelRotTwist > 0 
				momentTwist = momentTwist-phys->unldTw*relRotTwist;
				phys->unloadedTw = true; 
// 				cout<<"TWIST Plast UNLD"<<endl;
				if(TwistValue>=lastPlastTw){phys->unloadedTw = false;} 
			} 
			    
			else{momentTwist = momentTwist-phys->crpTw*relRotTwist;// loading, applying Creeping
				Vector3r AbsRot = de1->rot()-de2->rot();
				Real AbsTwist = (geom->normal.dot(AbsRot)*geom->normal).norm();
				lastPlastTw= TwistValue;
// 				cout<<"TWIST Creep momentTwist="<<momentTwist<<" AbsTwist="<<AbsTwist<<endl;
				if (AbsTwist>phys->phTwMax){ // Plastic rupture //FIXME: This line is not ok, need to find the compare the total(or just plastic) angular displacement to PhBmax, true meaning of relRotBend??
					momentTwist = Vector3r(0,0,0);
					phys->isBrokenTw = true;
				}
			} 
		}
		phys->moment_twist = momentTwist;
	}
	
// 	cout<<"moment Twist= "<<phys->moment_twist<<endl;

	
	// Apply moments now
	Vector3r moment = phys->moment_twist + phys->moment_bending;
	scene->forces.addTorque(id1,-moment);
	scene->forces.addTorque(id2, moment);			
	/// Moment law END ///

}

YADE_PLUGIN((Law2_ScGeom6D_InelastCohFrictPhys_CohesionMoment));
