/* LucScholtes2010  */

#include"JointedCohesiveFrictionalPM.hpp"
#include<core/Scene.hpp>
#include<pkg/dem/ScGeom.hpp>
#include<core/Omega.hpp>

YADE_PLUGIN((JCFpmMat)(JCFpmState)(JCFpmPhys)(Ip2_JCFpmMat_JCFpmMat_JCFpmPhys)(Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM));


/********************** Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM ****************************/
CREATE_LOGGER(Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM);

bool Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact){

	const int &id1 = contact->getId1();
	const int &id2 = contact->getId2();
	ScGeom* geom = static_cast<ScGeom*>(ig.get()); 
	JCFpmPhys* phys = static_cast<JCFpmPhys*>(ip.get());
	
	Body* b1 = Body::byId(id1,scene).get();
	Body* b2 = Body::byId(id2,scene).get();

	Real Dtensile=phys->FnMax/phys->kn;
	
	string fileCracks = "cracks_"+Key+".txt";
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
	contactPhysics->crossSection = Mathr::PI*pow(min(R1,R2),2);

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

JCFpmPhys::~JCFpmPhys(){}
