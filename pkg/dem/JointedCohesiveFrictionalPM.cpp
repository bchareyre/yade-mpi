/* LucScholtes2010  */

#include"JointedCohesiveFrictionalPM.hpp"
#include<core/Scene.hpp>
#include<pkg/dem/ScGeom.hpp>
#include<core/Omega.hpp>
#include<pkg/common/Sphere.hpp>

YADE_PLUGIN((JCFpmMat)(JCFpmState)(JCFpmPhys)(Ip2_JCFpmMat_JCFpmMat_JCFpmPhys)(Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM));


/********************** Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM ****************************/
CREATE_LOGGER(Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM);

static boost::mutex nearbyInts_mutex;
static boost::mutex clusterInts_mutex;

bool Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact){

	const int &id1 = contact->getId1();
	const int &id2 = contact->getId2();
	ScGeom* geom = static_cast<ScGeom*>(ig.get()); 
	JCFpmPhys* phys = static_cast<JCFpmPhys*>(ip.get());
	
	Body* b1 = Body::byId(id1,scene).get();
	Body* b2 = Body::byId(id2,scene).get();

	Real Dtensile=phys->FnMax/phys->kn;
	
	string fileCracks = "cracks_"+Key+".txt";
	string fileMoments = "moments_"+Key+".txt";
	/// Defines the interparticular distance used for computation
	Real D = 0;

	/*this is for setting the equilibrium distance between all cohesive elements at the first contact detection*/
	if ( contact->isFresh(scene) ) { 
	  phys->normalForce = Vector3r::Zero(); 
	  phys->shearForce = Vector3r::Zero();
	  if ((smoothJoint) && (phys->isOnJoint)) {
	    phys->jointNormal = geom->normal.dot(phys->jointNormal)*phys->jointNormal; //to set the joint normal colinear with the interaction normal
	    phys->jointNormal.normalize();
	    phys->initD = std::abs((b1->state->pos - b2->state->pos).dot(phys->jointNormal)); // to set the initial gap as the equilibrium gap
	  } else { 
	    phys->initD = geom->penetrationDepth; 
	  }
	}
	
	if ( smoothJoint && phys->isOnJoint ) {
	  if ( phys->more || ( phys-> jointCumulativeSliding > (2*min(geom->radius1,geom->radius2)) ) ) { 
	    if (!neverErase) return false; 
	    else {
	      phys->shearForce = Vector3r::Zero();
	      phys->normalForce = Vector3r::Zero();
	      phys->isCohesive =0;
	      phys->FnMax = 0;
	      phys->FsMax = 0;
	      return true;
	      }
	  } else { 
	    D = phys->initD - std::abs((b1->state->pos - b2->state->pos).dot(phys->jointNormal)); 
	  }
	} else { 
	  D = geom->penetrationDepth - phys->initD; 
	}
	
	phys->crackJointAperture = D<0? -D : 0.; // for DFNFlow

	if (!phys->momentBroken  && useStrainEnergy) phys->strainEnergy = 0.5*((pow(phys->normalForce.norm(),2)/phys->kn) + (pow(phys->shearForce.norm(),2)/phys->ks));
	else if (!phys->momentBroken && !useStrainEnergy) computeKineticEnergy(phys, b1, b2);

//Compute clustered acoustic emission events:
	if (recordMoments && !neverErase){ 
			cerr << "Acoustic emissions algorithm requires neverErase=True, changing value from False to True" << endl;
			neverErase=true;
	}
		
	if (phys->momentBroken && recordMoments && !phys->momentCalculated){
		if (phys->originalClusterEvent && !phys->computedCentroid) computeCentroid(phys);
		if (phys->originalClusterEvent) computeClusteredMoment(phys);
		
		if (phys->momentCalculated && phys->momentMagnitude!=0){
			std::ofstream file (fileMoments.c_str(), !momentsFileExist ? std::ios::trunc : std::ios::app);
			if(file.tellp()==0){ file <<"i p0 p1 p2 moment numInts eventNum time"<<endl; }
			file << boost::lexical_cast<string> ( scene->iter )<<" "<< boost::lexical_cast<string> ( phys->momentCentroid[0] ) <<" "<< boost::lexical_cast<string> ( phys->momentCentroid[1] ) <<" "<< boost::lexical_cast<string> ( phys->momentCentroid[2] ) <<" "<< boost::lexical_cast<string> ( phys->momentMagnitude ) << " " << boost::lexical_cast<string> ( phys->clusterInts.size() ) << " " << boost::lexical_cast<string> ( phys->eventNumber ) << " " << boost::lexical_cast<string> (scene->time) << endl;
			momentsFileExist=true;
		}
		
	}

	/* Determination of interaction */
	if (D < 0) { //tensile configuration
	  if ( !phys->isCohesive) {
	    if (!neverErase) return false;
	    else {
	      phys->shearForce = Vector3r::Zero();
	      phys->normalForce = Vector3r::Zero();
	      phys->isCohesive =0;
	      phys->FnMax = 0;
	      phys->FsMax = 0;
	      return true;
	    }
	  }
	  
	  if ( phys->isCohesive && (phys->FnMax>0) && (std::abs(D)>Dtensile) ) {
	    
	    nbTensCracks++;
	    phys->isCohesive = 0;
	    phys->FnMax = 0;
	    phys->FsMax = 0;
	    /// Do we need both the following lines?
	    phys->breakOccurred = true;  // flag to trigger remesh for DFNFlowEngine
	    phys->isBroken = true; // flag for DFNFlowEngine
	    
            // update body state with the number of broken bonds -> do we really need that?
	    JCFpmState* st1=dynamic_cast<JCFpmState*>(b1->state.get());
	    JCFpmState* st2=dynamic_cast<JCFpmState*>(b2->state.get());
            st1->nbBrokenBonds++;
	    st2->nbBrokenBonds++;
	    st1->damageIndex+=1.0/st1->nbInitBonds;
	    st2->damageIndex+=1.0/st2->nbInitBonds;
		phys->momentBroken = true;
            
            Real scalarNF=phys->normalForce.norm();
	    Real scalarSF=phys->shearForce.norm();
	    totalTensCracksE+=0.5*( ((scalarNF*scalarNF)/phys->kn) + ((scalarSF*scalarSF)/phys->ks) );
            totalCracksSurface += phys->crossSection;
	    
	    if (recordCracks){
                std::ofstream file (fileCracks.c_str(), !cracksFileExist ? std::ios::trunc : std::ios::app);
                if(file.tellp()==0){ file <<"iter time p0 p1 p2 type size norm0 norm1 norm2 nrg"<<endl; }
                Vector3r crackNormal=Vector3r::Zero();
                if ((smoothJoint) && (phys->isOnJoint)) { crackNormal=phys->jointNormal; } else {crackNormal=geom->normal;}
                file << boost::lexical_cast<string> ( scene->iter ) << " " << boost::lexical_cast<string> ( scene->time ) <<" "<< boost::lexical_cast<string> ( geom->contactPoint[0] ) <<" "<< boost::lexical_cast<string> ( geom->contactPoint[1] ) <<" "<< boost::lexical_cast<string> ( geom->contactPoint[2] ) <<" "<< 1 <<" "<< boost::lexical_cast<string> ( 0.5*(geom->radius1+geom->radius2) ) <<" "<< boost::lexical_cast<string> ( crackNormal[0] ) <<" "<< boost::lexical_cast<string> ( crackNormal[1] ) <<" "<< boost::lexical_cast<string> ( crackNormal[2] ) <<" "<< boost::lexical_cast<string> ( 0.5*( ((scalarNF*scalarNF)/phys->kn) + ((scalarSF*scalarSF)/phys->ks) ) ) <<endl;
	    }
        if (recordMoments && !phys->momentCalculated){
        	checkForCluster(phys, geom, b1, b2, contact);
			clusterInteractions(phys, contact);
			computeTemporalWindow(phys, b1, b2);
        }
	    cracksFileExist=true;
            
	    if (!neverErase) return false; 
	    else {
	      phys->shearForce = Vector3r::Zero();
	      phys->normalForce = Vector3r::Zero();
	      return true;
	    }
	  }
	}
	
	/* NormalForce */
	Real Fn = 0;
	Fn = phys->kn*D; 
        
	/* ShearForce */
	Vector3r& shearForce = phys->shearForce; 
	Real jointSliding=0;

	if ((smoothJoint) && (phys->isOnJoint)) {
	  
	  /// incremental formulation (OK?)
	  Vector3r relativeVelocity = (b2->state->vel - b1->state->vel); // angVel are not taken into account as particles on joint don't rotate ????
	  Vector3r slidingVelocity = relativeVelocity - phys->jointNormal.dot(relativeVelocity)*phys->jointNormal; 
	  Vector3r incrementalSliding = slidingVelocity*scene->dt;
	  shearForce -= phys->ks*incrementalSliding;
	  
	  jointSliding = incrementalSliding.norm();
	  phys->jointCumulativeSliding += jointSliding;
  
	} else {

	  shearForce = geom->rotate(phys->shearForce);
	  const Vector3r& incrementalShear = geom->shearIncrement();
	  shearForce -= phys->ks*incrementalShear;
	  
	}
	
	/* Mohr-Coulomb criterion */
	Real maxFs = phys->FsMax + Fn*phys->tanFrictionAngle;
	Real scalarShearForce = shearForce.norm();
               
	if (scalarShearForce > maxFs) {
	  if (scalarShearForce != 0)
	    shearForce*=maxFs/scalarShearForce;
	  else
	    shearForce=Vector3r::Zero();
	  if ((smoothJoint) && (phys->isOnJoint)) {phys->dilation=phys->jointCumulativeSliding*phys->tanDilationAngle-D; phys->initD+=(jointSliding*phys->tanDilationAngle);}

// 	  if (!phys->isCohesive) {
//             nbSlips++;
//             totalSlipE+=((1./phys->ks)*(trialForce-shearForce))/*plastic disp*/.dot(shearForce)/*active force*/;
//             
// 	    if ( (recordSlips) && (maxFs!=0) ) {
// 	    std::ofstream file (fileCracks.c_str(), !cracksFileExist ? std::ios::trunc : std::ios::app);
// 	    if(file.tellp()==0){ file <<"iter time p0 p1 p2 type size norm0 norm1 norm2 nrg"<<endl; }
// 	    Vector3r crackNormal=Vector3r::Zero();
// 	    if ((smoothJoint) && (phys->isOnJoint)) { crackNormal=phys->jointNormal; } else {crackNormal=geom->normal;}
// 	    file << boost::lexical_cast<string> ( scene->iter ) <<" " << boost::lexical_cast<string> ( scene->time ) <<" "<< boost::lexical_cast<string> ( geom->contactPoint[0] ) <<" "<< boost::lexical_cast<string> ( geom->contactPoint[1] ) <<" "<< boost::lexical_cast<string> ( geom->contactPoint[2] ) <<" "<< 0 <<" "<< boost::lexical_cast<string> ( 0.5*(geom->radius1+geom->radius2) ) <<" "<< boost::lexical_cast<string> ( crackNormal[0] ) <<" "<< boost::lexical_cast<string> ( crackNormal[1] ) <<" "<< boost::lexical_cast<string> ( crackNormal[2] ) <<" "<< boost::lexical_cast<string> ( ((1./phys->ks)*(trialForce-shearForce)).dot(shearForce) ) << endl;
// 	    }
// 	    cracksFileExist=true;    
// 	  }

	  if ( phys->isCohesive ) { 

	    nbShearCracks++;
	    phys->isCohesive = 0;
	    phys->FnMax = 0;
	    phys->FsMax = 0;
	    /// Do we need both the following lines?
	    phys->breakOccurred = true;  // flag to trigger remesh for DFNFlowEngine
	    phys->isBroken = true; // flag for DFNFlowEngine
	    phys->momentBroken = true;
	    // update body state with the number of broken bonds -> do we really need that?
	    JCFpmState* st1=dynamic_cast<JCFpmState*>(b1->state.get());
	    JCFpmState* st2=dynamic_cast<JCFpmState*>(b2->state.get());
	    st1->nbBrokenBonds++;
	    st2->nbBrokenBonds++;
	    st1->damageIndex+=1.0/st1->nbInitBonds;
	    st2->damageIndex+=1.0/st2->nbInitBonds;
          
	    Real scalarNF=phys->normalForce.norm();
	    Real scalarSF=phys->shearForce.norm();
	    totalShearCracksE+=0.5*( ((scalarNF*scalarNF)/phys->kn) + ((scalarSF*scalarSF)/phys->ks) );
            totalCracksSurface += phys->crossSection;
    
	    if (recordCracks){
	      std::ofstream file (fileCracks.c_str(), !cracksFileExist ? std::ios::trunc : std::ios::app);
	      if(file.tellp()==0){ file <<"iter time p0 p1 p2 type size norm0 norm1 norm2 nrg"<<endl; }
	      Vector3r crackNormal=Vector3r::Zero();
	      if ((smoothJoint) && (phys->isOnJoint)) { crackNormal=phys->jointNormal; } else {crackNormal=geom->normal;}
	      file << boost::lexical_cast<string> ( scene->iter ) << " " << boost::lexical_cast<string> ( scene->time ) <<" "<< boost::lexical_cast<string> ( geom->contactPoint[0] ) <<" "<< boost::lexical_cast<string> ( geom->contactPoint[1] ) <<" "<< boost::lexical_cast<string> ( geom->contactPoint[2] ) <<" "<< 2 <<" "<< boost::lexical_cast<string> ( 0.5*(geom->radius1+geom->radius2) ) <<" "<< boost::lexical_cast<string> ( crackNormal[0] ) <<" "<< boost::lexical_cast<string> ( crackNormal[1] ) <<" "<< boost::lexical_cast<string> ( crackNormal[2] ) <<" "<< boost::lexical_cast<string> ( 0.5*( ((scalarNF*scalarNF)/phys->kn) + ((scalarSF*scalarSF)/phys->ks) ) ) <<endl;
	    }
	    cracksFileExist=true;
	    
// 	    // option 1: delete contact whatsoever (if in compression, it will be detected as a new contact at the next timestep -> actually, not necesarily because of the near neighbour interaction: there could be a gap between the bonded particles and thus a broken contact may not be frictional at the next timestep if the detection is done for strictly contacting particles...) -> to TEST
//             if (!neverErase) return false;
//             else {
//                 phys->shearForce = Vector3r::Zero();
// 		phys->normalForce = Vector3r::Zero();
// 		return true;
//             }

        if (recordMoments && !phys->momentCalculated){
        	checkForCluster(phys, geom, b1, b2, contact);
			clusterInteractions(phys, contact);
			computeTemporalWindow(phys, b1, b2);
        }
            
            // option 2: delete contact if in tension
//	    shearForce *= Fn*phys->tanFrictionAngle/scalarShearForce; // now or at the next timestep? should not be very different -> to TEST
	    if ( D < 0 ) { // spheres do not touch
                if (!neverErase) return false;
                else {
                    phys->shearForce = Vector3r::Zero();
                    phys->normalForce = Vector3r::Zero();
                    return true;
                }
	    }
	    
	  }
	}
	
	/* Apply forces */
	if ((smoothJoint) && (phys->isOnJoint)) { phys->normalForce = Fn*phys->jointNormal; } else { phys->normalForce = Fn*geom->normal; }
	
	Vector3r f = phys->normalForce + shearForce;
	
	/// applyForceAtContactPoint computes torque also and, for now, we don't want rotation for particles on joint (some errors in calculation due to specific geometry) 
 	//applyForceAtContactPoint(f, geom->contactPoint, I->getId2(), b2->state->pos, I->getId1(), b1->state->pos, scene);
	scene->forces.addForce (id1,-f);
	scene->forces.addForce (id2, f);
	
	// simple solution to avoid torque computation for particles interacting on a smooth joint 
	if ( (phys->isOnJoint)&&(smoothJoint) ) return true;
	
	/// those lines are needed if rootBody->forces.addForce and rootBody->forces.addMoment are used instead of applyForceAtContactPoint -> NOTE need to check for accuracy!!!
	scene->forces.addTorque(id1,(geom->radius1-0.5*geom->penetrationDepth)* geom->normal.cross(-f));
	scene->forces.addTorque(id2,(geom->radius2-0.5*geom->penetrationDepth)* geom->normal.cross(-f));
	return true;
	
}

void Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM::computeKineticEnergy
	(JCFpmPhys* phys, Body* b1, Body* b2)
	{
	JCFpmState* state1 = YADE_CAST<JCFpmState*>(b1->state.get());
	JCFpmState* state2 = YADE_CAST<JCFpmState*>(b2->state.get());
	Real m1 = state1->mass; 
	Real m2 = state2->mass;
	Vector3r vel1 = state1->vel;
	Vector3r vel2 = state2->vel;
	Vector3r inert1 = state1->inertia;
	Vector3r inert2= state2->inertia;
	Vector3r angVel1 = state1->angVel;
	Vector3r angVel2 = state2->angVel;
	
	Real kineticEnergy1 = 0.5 * (m1*(vel1[0]*vel1[0]+vel1[1]*vel1[1]+vel1[2]*vel1[2]) 
		+ inert1[0]*(angVel1[0]*angVel1[0]+angVel1[1]*angVel1[1]+angVel1[2]*angVel1[2]));
	Real kineticEnergy2 = 0.5 * (m2*(vel2[0]*vel2[0]+vel2[1]*vel2[1]+vel2[2]*vel2[2]) 
		+ inert2[0]*(angVel2[0]*angVel2[0]+angVel2[1]*angVel2[1]+angVel2[2]*angVel2[2]));

	phys->kineticEnergy = kineticEnergy1 + kineticEnergy2;
}


// function used to parse through new interactions and only add unique ints to cluster list	
void Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM::addUniqueIntsToList(JCFpmPhys* phys, JCFpmPhys* nearbyPhys){
	unsigned int size = phys->nearbyInts.size();
	for (unsigned int i=0; i<nearbyPhys->nearbyInts.size(); i++){
		if (!nearbyPhys->nearbyInts[i]) continue;
		bool pushBack = true;
		for (unsigned int j=0; j<size; j++){
			if (!phys->nearbyInts[j]) continue;
			if (phys->nearbyInts[j] == nearbyPhys->nearbyInts[i]) {
				pushBack = false;
				break;
			}
		}
		boost::mutex::scoped_lock lock(nearbyInts_mutex);
		if (pushBack && nearbyPhys->nearbyInts[i]){
			phys->nearbyInts.push_back(nearbyPhys->nearbyInts[i]);
			JCFpmPhys* nrgPhys = YADE_CAST<JCFpmPhys*> (nearbyPhys->nearbyInts[i]->phys.get()); 
			phys->momentEnergy += (useStrainEnergy ? nrgPhys->strainEnergy : nrgPhys->kineticEnergy);  // while we are here we update the reference strain (or kinetic) energy by adding the strain (or kinetic) energy of the newly added ints
		}
			
	}
}




// function used for clustering broken bonds and nearby interactions
void Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM::clusterInteractions(JCFpmPhys* phys, Interaction* contact){
	JCFpmPhys* originalPhys = YADE_CAST<JCFpmPhys*>(phys->originalEvent->phys.get());
	addUniqueIntsToList(originalPhys, phys);  //NEED TO PUSHBACK ONLY UNIQUE INTS. we don't want a list with duplicate events (also updates reference strain)
	phys->interactionsAdded = true;
	originalPhys->elapsedIter = 1;  // reset the temporal window? do we want this?
	//originalPhys->firstMomentCalc=true; // do we need a new reference strain energy for the calculation of strain energy change?
	phys->momentMagnitude = 0; // dirty way to avoid recording these clustered events twice? maybe dont need this if proper recording is applied
	originalPhys->computedCentroid=false;  // set flag to compute a new centroid since we added more ints
	boost::mutex::scoped_lock lock(clusterInts_mutex);
	originalPhys->clusterInts.push_back(contact);  // add this broken interaction to list of broken bonds in this event
} 

// function used to check if the newly broken bond belongs in a cluster or not, if so attach to proper cluster and set appropriate flags
void Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM::checkForCluster(JCFpmPhys* phys, ScGeom* geom, Body* b1, Body* b2, Interaction* contact){

	const shared_ptr<Shape>& sphere1 = b1->shape;
	const shared_ptr<Shape>& sphere2 = b2->shape;
	const Real sphereRadius1 = static_cast<Sphere*>(sphere1.get())->radius;
	const Real sphereRadius2 = static_cast<Sphere*>(sphere2.get())->radius;
	const Real avgDiameter = (sphereRadius1+sphereRadius2);
	Vector3r& brokenInteractionLocation = geom->contactPoint;
	phys->nearbyFound=0;

	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
	//#endif
		JCFpmPhys* nearbyPhys;
		const ScGeom* nearbyGeom;
		if (!I || !I->geom.get() || !I->phys.get()) continue;
		if (I && I->isReal() && JCFpmPhys::getClassIndexStatic()==I->phys->getClassIndex()){
			nearbyPhys = YADE_CAST<JCFpmPhys*>(I->phys.get());
			if (!nearbyPhys) continue;
			if (I->geom.get() /*&& !nearbyPhys->momentBroken*/){
				nearbyGeom = YADE_CAST<ScGeom*> (I->geom.get());
                    		if (!nearbyGeom) continue;
				Vector3r nearbyInteractionLocation = nearbyGeom->contactPoint;
				Vector3r proximityVector = nearbyInteractionLocation-brokenInteractionLocation;
				Real proximity = proximityVector.norm();
				
				// this logic is finding interactions within a radius of the broken one, and identifiying if it is an original event or if it belongs in a cluster
				if (proximity < avgDiameter*momentRadiusFactor && proximity != 0){
					if (nearbyPhys->originalClusterEvent && !nearbyPhys->momentCalculated && !phys->clusteredEvent && clusterMoments) {
						phys->eventNumber = nearbyPhys->eventNumber; 
						phys->clusteredEvent = true;
						phys->originalEvent = I.get();
					} else if (nearbyPhys->clusteredEvent && !phys->clusteredEvent && clusterMoments){
						JCFpmPhys* originalPhys = YADE_CAST<JCFpmPhys*>(nearbyPhys->originalEvent->phys.get());
						if (!originalPhys->momentCalculated){
							phys->eventNumber = nearbyPhys->eventNumber;
							phys->clusteredEvent = true;
							phys->originalEvent = nearbyPhys->originalEvent;
						}
					} 

					if (nearbyPhys->momentBroken) continue;
					phys->nearbyInts.push_back(I.get());
				}
			}
		}
	}
	if (!phys->clusteredEvent) {
		phys->originalClusterEvent = true; // if not clustered, its an original event. We use this interaction as the master for the cluster. Its list of nearbyInts will expand if other ints break nearby. 
		phys->originalEvent = contact;
		eventNumber += 1;
		phys->eventNumber = eventNumber;
	}
	phys->checkedForCluster = true;
}

// function cycles through the list of interactions associated with a cluster and computes moment
void Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM::computeClusteredMoment(JCFpmPhys* phys){
	Real totalMomentEnergy = 0;
	Real momentEnergyChange = 0;
	for (unsigned int i=0; i<phys->nearbyInts.size(); i++){
		const JCFpmPhys* nearbyPhys;
		if (!phys->nearbyInts[i] || !phys->nearbyInts[i]->geom.get() || !phys->nearbyInts[i]->phys.get()) continue;
		nearbyPhys = YADE_CAST<JCFpmPhys*>(phys->nearbyInts[i]->phys.get());
		if (!nearbyPhys) continue;
		totalMomentEnergy += (useStrainEnergy ? nearbyPhys->strainEnergy : nearbyPhys->kineticEnergy);
	}
	if(phys->firstMomentCalc){
		phys->momentEnergy = totalMomentEnergy;
		phys->firstMomentCalc = false;
	}
	momentEnergyChange = totalMomentEnergy - phys->momentEnergy;
	phys->elapsedIter += 1;
	if (momentEnergyChange > phys->momentEnergyChange) phys->momentEnergyChange = momentEnergyChange;
	if (phys->elapsedIter >= phys->temporalWindow){ // the elapsed time should reflect 20*particlediameters radius Hazzard and Damjanac 2013
		phys->originalClusterEvent=false; // this event no longer exists, so we need to allow other new events to occur nearby.    
		if(phys->momentEnergyChange!=0) phys->momentMagnitude = (2./3.)*log(phys->momentEnergyChange*momentFudgeFactor)-3.2; 
		phys->momentCalculated=true;	
 //empirical equation for energy magnitude (Hazzard and Damjanac 2013) 
		//if(phys->momentStrainEnergyChange==0) cout<<"avgDiameter " << avgDiameter << " found nearby interaciton? " << phys->nearbyFound << "over " << phys->elapsedIter << " iterations" <<endl;  // debugging. It appears some strain energy searches yield decreases of strain energy in neighbor hood. We are handling these by assining a 0 magnitude...but that seems wrong. Turns out these are just very very small events. There is actually no change of strain around them. Due to weibull dist int areas?
	}
					
}

// function used to compute the temporal window based on the PWave velocity of the medium
void Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM::computeTemporalWindow
	(JCFpmPhys* phys, Body* b1, Body* b2)
	{
	const shared_ptr<Shape>& sphere1 = b1->shape;
	const shared_ptr<Shape>& sphere2 = b2->shape;
	const Real sphereRadius1 = static_cast<Sphere*>(sphere1.get())->radius;
	const Real sphereRadius2 = static_cast<Sphere*>(sphere2.get())->radius;	
	const Real avgDiameter = (sphereRadius1+sphereRadius2);
	const Real spatialWindow = avgDiameter*momentRadiusFactor;
	const shared_ptr<ElastMat>& elasticMat1 = YADE_PTR_DYN_CAST<ElastMat>(b1->material);
	const shared_ptr<ElastMat>& elasticMat2 = YADE_PTR_DYN_CAST<ElastMat>(b2->material);
	const Real velocityP1 = sqrt(elasticMat1->young/elasticMat1->density);
	const Real velocityP2 = sqrt(elasticMat2->young/elasticMat2->density);

	phys->temporalWindow = floor(spatialWindow/(max(velocityP1, velocityP2)*scene->dt));
}
	
	



// function computes the centroid of a cluster
void Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM::computeCentroid(JCFpmPhys* phys){
	JCFpmPhys* originalPhys = YADE_CAST<JCFpmPhys*>(phys->originalEvent->phys.get());
	Vector3r summedLocations = Vector3r::Zero();
	for (unsigned int i=0; i<originalPhys->clusterInts.size(); i++){
		ScGeom* nearbyGeom;
		if (!originalPhys->clusterInts[i]) continue;
		if (originalPhys->clusterInts[i]->geom.get()){
			nearbyGeom = YADE_CAST<ScGeom*> (originalPhys->clusterInts[i]->geom.get());
			Vector3r nearbyInteractionLocation = nearbyGeom->contactPoint;
			summedLocations += nearbyInteractionLocation;
		}
	}
	originalPhys->momentCentroid = summedLocations/originalPhys->clusterInts.size(); // new location of event is average of all clustered events
	originalPhys->computedCentroid = true;
	
}

CREATE_LOGGER(Ip2_JCFpmMat_JCFpmMat_JCFpmPhys);

void Ip2_JCFpmMat_JCFpmMat_JCFpmPhys::go(const shared_ptr<Material>& b1, const shared_ptr<Material>& b2, const shared_ptr<Interaction>& interaction){

	/* avoid updates of interaction if it already exists */
	if( interaction->phys ) return; 

	ScGeom* geom=dynamic_cast<ScGeom*>(interaction->geom.get());
	assert(geom);

	const shared_ptr<JCFpmMat>& yade1 = YADE_PTR_CAST<JCFpmMat>(b1);
	const shared_ptr<JCFpmMat>& yade2 = YADE_PTR_CAST<JCFpmMat>(b2);
	JCFpmState* st1=dynamic_cast<JCFpmState*>(Body::byId(interaction->getId1(),scene)->state.get());
	JCFpmState* st2=dynamic_cast<JCFpmState*>(Body::byId(interaction->getId2(),scene)->state.get());
	
	shared_ptr<JCFpmPhys> contactPhysics(new JCFpmPhys()); 
	
	/* From material properties */
	Real E1 	= yade1->young;
	Real E2 	= yade2->young;
	Real v1 	= yade1->poisson;
	Real v2 	= yade2->poisson;
	Real f1 	= yade1->frictionAngle;
	Real f2 	= yade2->frictionAngle;
        Real rf1 	= yade1->residualFrictionAngle>=0? yade1->residualFrictionAngle: yade1->frictionAngle;
	Real rf2 	= yade2->residualFrictionAngle>=0? yade2->residualFrictionAngle: yade2->frictionAngle;
	Real SigT1	= yade1->tensileStrength;
	Real SigT2	= yade2->tensileStrength;
	Real Coh1	= yade1->cohesion;
	Real Coh2	= yade2->cohesion;

	/* From interaction geometry */
	Real R1= geom->radius1;
	Real R2= geom->radius2;

	// control the radius used for cross-sectional area computation (adding rock heterogeneity)
	if (xSectionWeibullShapeParameter>0) {
 		distributeCrossSectionsWeibull(contactPhysics, R1, R2);	
	} else {
	contactPhysics->crossSection = Mathr::PI*pow(min(R1,R2),2); 
	}

	/* Pass values to JCFpmPhys. In case of a "jointed" interaction, the following values will be replaced by other ones later (in few if(){} blocks)*/
	
	// elastic properties
	contactPhysics->kn = 2.*E1*R1*E2*R2/(E1*R1+E2*R2);
        ( (v1==0)&&(v2==0) )? contactPhysics->ks=0 : contactPhysics->ks = 2.*E1*R1*v1*E2*R2*v2/(E1*R1*v1+E2*R2*v2);
	
	// cohesive properties
	///to set if the contact is cohesive or not
	if ( ((cohesiveTresholdIteration < 0) || (scene->iter < cohesiveTresholdIteration)) && (std::min(SigT1,SigT2)>0 || std::min(Coh1,Coh2)>0) && (yade1->type == yade2->type)){ 
	  contactPhysics->isCohesive=true;
	  st1->nbInitBonds++;
	  st2->nbInitBonds++;
	}
	
	if ( contactPhysics->isCohesive ) {
	  contactPhysics->FnMax = std::min(SigT1,SigT2)*contactPhysics->crossSection;
	  contactPhysics->FsMax = std::min(Coh1,Coh2)*contactPhysics->crossSection;
	}
	// do we need that?
// 	else {
// 	  contactPhysics->FnMax = 0.;
// 	  contactPhysics->FsMax = 0.;
// 	}
	
        // frictional properties      
        contactPhysics->isCohesive? contactPhysics->tanFrictionAngle = std::tan(std::min(f1,f2)) : contactPhysics->tanFrictionAngle = std::tan(std::min(rf1,rf2));

	/// +++ Jointed interactions ->NOTE: geom->normal is oriented from 1 to 2 / jointNormal from plane to sphere 
	if ( st1->onJoint && st2->onJoint )
	{
		if ( (((st1->jointNormal1.cross(st2->jointNormal1)).norm()<0.1) && (st1->jointNormal1.dot(st2->jointNormal1)<0)) || (((st1->jointNormal1.cross(st2->jointNormal2)).norm()<0.1) && (st1->jointNormal1.dot(st2->jointNormal2)<0)) || (((st1->jointNormal1.cross(st2->jointNormal3)).norm()<0.1) && (st1->jointNormal1.dot(st2->jointNormal3)<0)) )
		{
		  contactPhysics->isOnJoint = true;
		  contactPhysics->jointNormal = st1->jointNormal1;
		}
		else if ( (((st1->jointNormal2.cross(st2->jointNormal1)).norm()<0.1) && (st1->jointNormal2.dot(st2->jointNormal1)<0)) || (((st1->jointNormal2.cross(st2->jointNormal2)).norm()<0.1) && (st1->jointNormal2.dot(st2->jointNormal2)<0)) || (((st1->jointNormal2.cross(st2->jointNormal3)).norm()<0.1) && (st1->jointNormal2.dot(st2->jointNormal3)<0)) )
		{
		  contactPhysics->isOnJoint = true;
		  contactPhysics->jointNormal = st1->jointNormal2;
		}
		else if ( (((st1->jointNormal3.cross(st2->jointNormal1)).norm()<0.1) && (st1->jointNormal3.dot(st2->jointNormal1)<0)) || (((st1->jointNormal3.cross(st2->jointNormal2)).norm()<0.1) && (st1->jointNormal3.dot(st2->jointNormal2)<0)) || (((st1->jointNormal3.cross(st2->jointNormal3)).norm()<0.1) && (st1->jointNormal3.dot(st2->jointNormal3)<0)) )
		{
		  contactPhysics->isOnJoint = true;
		  contactPhysics->jointNormal = st1->jointNormal3;
		}
		else if ( (st1->joint>3 || st2->joint>3) && ( ( ((st1->jointNormal1.cross(st2->jointNormal1)).norm()>0.1) && ((st1->jointNormal1.cross(st2->jointNormal2)).norm()>0.1) && ((st1->jointNormal1.cross(st2->jointNormal3)).norm()>0.1) ) || ( ((st1->jointNormal2.cross(st2->jointNormal1)).norm()>0.1) && ((st1->jointNormal2.cross(st2->jointNormal2)).norm()>0.1) && ((st1->jointNormal2.cross(st2->jointNormal3)).norm()>0.1) ) || ( ((st1->jointNormal3.cross(st2->jointNormal1)).norm()>0.1) && ((st1->jointNormal3.cross(st2->jointNormal2)).norm()>0.1) && ((st1->jointNormal3.cross(st2->jointNormal3)).norm()>0.1) ) ) )  {  contactPhysics->isOnJoint = true; contactPhysics->more = true; contactPhysics->jointNormal = geom->normal; }
	}
	
	///to specify joint properties 
	if ( contactPhysics->isOnJoint ) {
			Real jf1 	= yade1->jointFrictionAngle;
			Real jf2 	= yade2->jointFrictionAngle;
			Real jkn1 	= yade1->jointNormalStiffness;
			Real jkn2 	= yade2->jointNormalStiffness;
			Real jks1 	= yade1->jointShearStiffness;
			Real jks2 	= yade2->jointShearStiffness;
			Real jdil1 	= yade1->jointDilationAngle;
			Real jdil2 	= yade2->jointDilationAngle;
			Real jcoh1 	= yade1->jointCohesion;
			Real jcoh2 	= yade2->jointCohesion;
			Real jSigT1	= yade1->jointTensileStrength;
			Real jSigT2	= yade2->jointTensileStrength;
			
			contactPhysics->tanFrictionAngle = std::tan(std::min(jf1,jf2));
			
			//contactPhysics->kn = jointNormalStiffness*2.*R1*R2/(R1+R2); // very first expression from Luc
			//contactPhysics->kn = (jkn1+jkn2)/2.0*2.*R1*R2/(R1+R2); // after putting jointNormalStiffness in material
			contactPhysics->kn = ( jkn1 + jkn2 ) /2.0 * contactPhysics->crossSection; // for a size independant expression
			contactPhysics->ks = ( jks1 + jks2 ) /2.0 * contactPhysics->crossSection; // for a size independant expression
			
			contactPhysics->tanDilationAngle = std::tan(std::min(jdil1,jdil2));
		  
			///to set if the contact is cohesive or not
			if ( ((cohesiveTresholdIteration < 0) || (scene->iter < cohesiveTresholdIteration)) && (std::min(jcoh1,jcoh2)>0 || std::min(jSigT1,jSigT2)>0) ) {
                            contactPhysics->isCohesive=true;
                            st1->nbInitBonds++;
                            st2->nbInitBonds++;
			} 
			else { contactPhysics->isCohesive=false; contactPhysics->FnMax=0; contactPhysics->FsMax=0; }
		  
			if ( contactPhysics->isCohesive ) {
                            contactPhysics->FnMax = std::min(jSigT1,jSigT2)*contactPhysics->crossSection;
                            contactPhysics->FsMax = std::min(jcoh1,jcoh2)*contactPhysics->crossSection;
			}
	}
	interaction->phys = contactPhysics;
}

void Ip2_JCFpmMat_JCFpmMat_JCFpmPhys::distributeCrossSectionsWeibull(shared_ptr<JCFpmPhys> contactPhysics, Real R1, Real R2){
	std::random_device rd;
	std::mt19937 e2(rd());
	std::weibull_distribution<Real> weibullDistribution(xSectionWeibullShapeParameter, xSectionWeibullScaleParameter);
	Real correction = weibullDistribution(e2);
	if (correction < weibullCutOffMin) correction = weibullCutOffMin;
	else if (correction > weibullCutOffMax) correction = weibullCutOffMax;
	Real interactingRadius = correction*min(R1, R2);  // correcting radius to account for grain interactions
	contactPhysics->crossSection = Mathr::PI*pow(interactingRadius,2);
}

JCFpmPhys::~JCFpmPhys(){}
