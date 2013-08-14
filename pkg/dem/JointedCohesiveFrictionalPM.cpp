/* LucScholtes2010  */

#include"JointedCohesiveFrictionalPM.hpp"
#include<yade/core/Scene.hpp>
#include<yade/pkg/dem/ScGeom.hpp>
#include<yade/core/Omega.hpp>

YADE_PLUGIN((JCFpmMat)(JCFpmState)(JCFpmPhys)(Ip2_JCFpmMat_JCFpmMat_JCFpmPhys)(Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM));


/********************** Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM ****************************/
CREATE_LOGGER(Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM);

void Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact){

	const int &id1 = contact->getId1();
	const int &id2 = contact->getId2();
	ScGeom* geom = static_cast<ScGeom*>(ig.get()); 
	JCFpmPhys* phys = static_cast<JCFpmPhys*>(ip.get());
	
	Body* b1 = Body::byId(id1,scene).get();
	Body* b2 = Body::byId(id2,scene).get();

	Real Dtensile=phys->FnMax/phys->kn;
	
	/// Defines the interparticular distance used for computation
	Real D = 0;

	/*this is for setting the equilibrium distance between all cohesive elements in the first contact detection*/
	if ( contact->isFresh(scene) ) { 
	  phys->normalForce = Vector3r::Zero(); 
	  phys->shearForce = Vector3r::Zero();
	  if ((smoothJoint) && (phys->isOnJoint)) {
	    phys->jointNormal = geom->normal.dot(phys->jointNormal)*phys->jointNormal; //to set the joint normal colinear with the interaction normal
	    phys->jointNormal.normalize();
	    phys->initD = abs((b1->state->pos - b2->state->pos).dot(phys->jointNormal)); // to set the initial gap as the equilibrium gap
	  } else { 
	    phys->initD = geom->penetrationDepth; 
	  }
	}
	
	if ( smoothJoint && phys->isOnJoint ) {
	  if ( phys->more || ( phys->jointCumulativeSliding > (2*min(geom->radius1,geom->radius2)) ) ) { 
	    scene->interactions->requestErase(contact->getId1(),contact->getId2()); return; 
	    } else { 
	    D = phys->initD - abs((b1->state->pos - b2->state->pos).dot(phys->jointNormal)); 
	    }
	} else { 
	  D = geom->penetrationDepth - phys->initD; 
	}

	/* Determination of interaction */
	if (D < 0) { //spheres do not touch 
	  if ( !phys->isCohesive ) { scene->interactions->requestErase(contact->getId1(),contact->getId2()); return; }
	  
	  if ( phys->isCohesive && (phys->FnMax>0) && (abs(D)>Dtensile) ) {
	    
	    // update body state with the number of broken bonds
	    JCFpmState* st1=dynamic_cast<JCFpmState*>(b1->state.get());
	    JCFpmState* st2=dynamic_cast<JCFpmState*>(b2->state.get());
	    st1->tensBreak+=1;
	    st2->tensBreak+=1;
	    
    	    // create a text file to record properties of the broken bond (iteration, position, type (tensile), cross section and contact normal orientation)
	    if (recordCracks){
	      std::ofstream file ("cracks.txt", !cracksFileExist ? std::ios::trunc : std::ios::app);
	      if(file.tellp()==0){ file <<"i p0 p1 p2 t s norm0 norm1 norm2"<<endl; }
	      Vector3r crackNormal=Vector3r::Zero();
	      if ((smoothJoint) && (phys->isOnJoint)) { crackNormal=phys->jointNormal; } else {crackNormal=geom->normal;}
	      file << lexical_cast<string> ( scene->iter )<<" "<< lexical_cast<string> ( geom->contactPoint[0] ) <<" "<< lexical_cast<string> ( geom->contactPoint[1] ) <<" "<< lexical_cast<string> ( geom->contactPoint[2] ) <<" "<< 0 <<" "<< lexical_cast<string> ( 0.5*(geom->radius1+geom->radius2) ) <<" "<< lexical_cast<string> ( crackNormal[0] ) <<" "<< lexical_cast<string> ( crackNormal[1] ) <<" "<< lexical_cast<string> ( crackNormal[2] ) << endl;
	    }
	    cracksFileExist=true;

	    // delete contact
	    scene->interactions->requestErase(contact->getId1(),contact->getId2()); return;
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
	
	/* Morh-Coulomb criterion */
	Real maxFs = phys->FsMax + Fn*phys->tanFrictionAngle;
	Real scalarShearForce = shearForce.norm();
	  
	if (scalarShearForce > maxFs) {
	  shearForce*=maxFs/scalarShearForce;
	  if ((smoothJoint) && (phys->isOnJoint)) {phys->dilation=phys->jointCumulativeSliding*phys->tanDilationAngle-D; phys->initD+=(jointSliding*phys->tanDilationAngle);}
	  // take into account shear cracking -> are those lines critical? -> TODO testing with and without
	  if ( phys->isCohesive ) { 

	    // update body state with the number of broken bonds
	    JCFpmState* st1=dynamic_cast<JCFpmState*>(b1->state.get());
	    JCFpmState* st2=dynamic_cast<JCFpmState*>(b2->state.get());
	    st1->shearBreak+=1;
	    st2->shearBreak+=1;

	    // create a text file to record properties of the broken bond (iteration, position, type (shear), cross section and contact normal orientation)
	    if (recordCracks){
	      std::ofstream file ("cracks.txt", !cracksFileExist ? std::ios::trunc : std::ios::app);
	      if(file.tellp()==0){ file <<"i p0 p1 p2 t s norm0 norm1 norm2"<<endl; }
	      Vector3r crackNormal=Vector3r::Zero();
	      if ((smoothJoint) && (phys->isOnJoint)) { crackNormal=phys->jointNormal; } else {crackNormal=geom->normal;}
	      file << lexical_cast<string> ( scene->iter )<<" "<< lexical_cast<string> ( geom->contactPoint[0] ) <<" "<< lexical_cast<string> ( geom->contactPoint[1] ) <<" "<< lexical_cast<string> ( geom->contactPoint[2] ) <<" "<< 1 <<" "<< lexical_cast<string> ( 0.5*(geom->radius1+geom->radius2) ) <<" "<< lexical_cast<string> ( crackNormal[0] ) <<" "<< lexical_cast<string> ( crackNormal[1] ) <<" "<< lexical_cast<string> ( crackNormal[2] ) << endl;
	    }
	    cracksFileExist=true;
	    
	    // delete contact if in tension, set the contact properties to friction if in compression
	    if ( D < 0 ) {
	      scene->interactions->requestErase(contact->getId1(),contact->getId2()); return;
	    } else {
	      phys->FnMax = 0;
	      phys->FsMax = 0;
	      phys->isCohesive=false;
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
	if ( (phys->isOnJoint)&&(smoothJoint) ) return;
	
	/// those lines are needed if rootBody->forces.addForce and rootBody->forces.addMoment are used instead of applyForceAtContactPoint -> NOTE need to check for accuracy!!!
	scene->forces.addTorque(id1,(geom->radius1-0.5*geom->penetrationDepth)* geom->normal.cross(-f));
	scene->forces.addTorque(id2,(geom->radius2-0.5*geom->penetrationDepth)* geom->normal.cross(-f));
	
}

CREATE_LOGGER(Ip2_JCFpmMat_JCFpmMat_JCFpmPhys);

void Ip2_JCFpmMat_JCFpmMat_JCFpmPhys::go(const shared_ptr<Material>& b1, const shared_ptr<Material>& b2, const shared_ptr<Interaction>& interaction){

	/* avoid updates of interaction if it already exists */
	if( interaction->phys ) return; 

	ScGeom* geom=dynamic_cast<ScGeom*>(interaction->geom.get());
	assert(geom);

	const shared_ptr<JCFpmMat>& yade1 = YADE_PTR_CAST<JCFpmMat>(b1);
	const shared_ptr<JCFpmMat>& yade2 = YADE_PTR_CAST<JCFpmMat>(b2);
	
	shared_ptr<JCFpmPhys> contactPhysics(new JCFpmPhys()); 
	
	/* From interaction physics */
	Real E1 	= yade1->young;
	Real E2 	= yade2->young;
	Real f1 	= yade1->frictionAngle;
	Real f2 	= yade2->frictionAngle;

	/* From interaction geometry */
	Real R1= geom->radius1;
	Real R2= geom->radius2;
	contactPhysics->crossSection = Mathr::PI*pow(min(R1,R2),2);

	/* Pass values to JCFpmPhys */
	
	// frictional properties
	contactPhysics->kn = 2.*E1*R1*E2*R2/(E1*R1+E2*R2);
	contactPhysics->ks = alpha*contactPhysics->kn;
	contactPhysics->frictionAngle = std::min(f1,f2); 
	contactPhysics->tanFrictionAngle = std::tan(contactPhysics->frictionAngle);
	
	// cohesive properties
	///to set if the contact is cohesive or not
	if ((scene->iter < cohesiveTresholdIteration) && (tensileStrength>0 || cohesion>0) && (yade1->type == yade2->type)){ contactPhysics->isCohesive=true; }
	
	if ( contactPhysics->isCohesive ) {
	  contactPhysics->FnMax = tensileStrength*contactPhysics->crossSection;
	  contactPhysics->FsMax = cohesion*contactPhysics->crossSection;
	}

	/// +++ Jointed interactions ->NOTE: geom->normal is oriented from 1 to 2 / jointNormal from plane to sphere 
	if ( yade1->onJoint && yade2->onJoint ) {

		if ( (((yade1->jointNormal1.cross(yade2->jointNormal1)).norm()<0.1) && (yade1->jointNormal1.dot(yade2->jointNormal1)<0)) || (((yade1->jointNormal1.cross(yade2->jointNormal2)).norm()<0.1) && (yade1->jointNormal1.dot(yade2->jointNormal2)<0)) || (((yade1->jointNormal1.cross(yade2->jointNormal3)).norm()<0.1) && (yade1->jointNormal1.dot(yade2->jointNormal3)<0)) ) { contactPhysics->isOnJoint = true; contactPhysics->jointNormal = yade1->jointNormal1; }
		else if ( (((yade1->jointNormal2.cross(yade2->jointNormal1)).norm()<0.1) && (yade1->jointNormal2.dot(yade2->jointNormal1)<0)) || (((yade1->jointNormal2.cross(yade2->jointNormal2)).norm()<0.1) && (yade1->jointNormal2.dot(yade2->jointNormal2)<0)) || (((yade1->jointNormal2.cross(yade2->jointNormal3)).norm()<0.1) && (yade1->jointNormal2.dot(yade2->jointNormal3)<0)) ) { contactPhysics->isOnJoint = true; contactPhysics->jointNormal = yade1->jointNormal2; }
		else if ( (((yade1->jointNormal3.cross(yade2->jointNormal1)).norm()<0.1) && (yade1->jointNormal3.dot(yade2->jointNormal1)<0)) || (((yade1->jointNormal3.cross(yade2->jointNormal2)).norm()<0.1) && (yade1->jointNormal3.dot(yade2->jointNormal2)<0)) || (((yade1->jointNormal3.cross(yade2->jointNormal3)).norm()<0.1) && (yade1->jointNormal3.dot(yade2->jointNormal3)<0)) ) { contactPhysics->isOnJoint = true; contactPhysics->jointNormal = yade1->jointNormal3; }
		else if ( (yade1->joint>3 || yade2->joint>3) && ( ( ((yade1->jointNormal1.cross(yade2->jointNormal1)).norm()>0.1) && ((yade1->jointNormal1.cross(yade2->jointNormal2)).norm()>0.1) && ((yade1->jointNormal1.cross(yade2->jointNormal3)).norm()>0.1) ) || ( ((yade1->jointNormal2.cross(yade2->jointNormal1)).norm()>0.1) && ((yade1->jointNormal2.cross(yade2->jointNormal2)).norm()>0.1) && ((yade1->jointNormal2.cross(yade2->jointNormal3)).norm()>0.1) ) || ( ((yade1->jointNormal3.cross(yade2->jointNormal1)).norm()>0.1) && ((yade1->jointNormal3.cross(yade2->jointNormal2)).norm()>0.1) && ((yade1->jointNormal3.cross(yade2->jointNormal3)).norm()>0.1) ) ) )  { contactPhysics->isOnJoint = true; contactPhysics->more = true; contactPhysics->jointNormal = geom->normal; }
	}
	
	///to specify joint properties 
	if ( contactPhysics->isOnJoint ) {
			contactPhysics->frictionAngle = (jointFrictionAngle>=0 ? jointFrictionAngle : std::min(f1,f2)); 
			contactPhysics->tanFrictionAngle = std::tan(contactPhysics->frictionAngle);
			contactPhysics->kn = jointNormalStiffness*2.*R1*R2/(R1+R2);
			contactPhysics->ks = jointShearStiffness*2.*R1*R2/(R1+R2); 
			contactPhysics->dilationAngle = jointDilationAngle;
			contactPhysics->tanDilationAngle = std::tan(contactPhysics->dilationAngle);
		  
			///to set if the contact is cohesive or not
			if ((scene->iter < cohesiveTresholdIteration) && (jointCohesion>0 || jointTensileStrength>0)) { contactPhysics->isCohesive=true; } 
			else { contactPhysics->isCohesive=false; contactPhysics->FnMax=0; contactPhysics->FsMax=0; }
		  
			if ( contactPhysics->isCohesive ) {
				contactPhysics->FnMax = jointTensileStrength*contactPhysics->crossSection;
				contactPhysics->FsMax = jointCohesion*contactPhysics->crossSection;
			}
	}
	interaction->phys = contactPhysics;
}

JCFpmPhys::~JCFpmPhys(){}