/* LucScholtes2010  */

#include"CohesiveFrictionalPM.hpp"
#include<yade/core/Scene.hpp>
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/core/Omega.hpp>

YADE_PLUGIN((CFpmMat)(CFpmState)(CFpmPhys)(Ip2_CFpmMat_CFpmMat_CFpmPhys)(Law2_ScGeom_CFpmPhys_CohesiveFrictionalPM));

/********************** Law2_ScGeom_CFpmPhys_CohesiveFrictionalPM ****************************/
CREATE_LOGGER(Law2_ScGeom_CFpmPhys_CohesiveFrictionalPM);

void Law2_ScGeom_CFpmPhys_CohesiveFrictionalPM::go(shared_ptr<InteractionGeometry>& ig, shared_ptr<InteractionPhysics>& ip, Interaction* contact, Scene* rootBody){
	
	ScGeom* geom = static_cast<ScGeom*>(ig.get()); 
	CFpmPhys* phys = static_cast<CFpmPhys*>(ip.get());
	int id1 = contact->getId1();
        int id2 = contact->getId2();
	shared_ptr<BodyContainer>& bodies = rootBody->bodies;
	Body* b1 = ( *bodies ) [id1].get();
	Body* b2 = ( *bodies ) [id2].get();
	Real dt = Omega::instance().getTimeStep();
	
	Real displN = geom->penetrationDepth; // NOTE: the sign for penetrationdepth is different from ScGeom and Dem3DofGeom: geom->penetrationDepth>0 when spheres interpenetrate
	Real Dtensile=phys->FnMax/phys->kn;
	Real Dsoftening = phys->strengthSoftening*Dtensile; 
	
	/*to set the equilibrium distance between all cohesive elements when they first meet -> stress-free assembly*/
	if ( contact->isFresh(rootBody) ) { phys->initD = displN; phys->normalForce = Vector3r::Zero(); phys->shearForce = Vector3r::Zero();}
	
	Real D = displN - phys->initD; // interparticular distance is computed depending on the equilibrium distance (which is 0 for non cohesive interactions)
	
	/* Determination of interaction */
	if (D < 0){ //spheres do not touch 
	  if (!phys->isCohesive){ rootBody->interactions->requestErase(contact->getId1(),contact->getId2()); return; } // destroy the interaction before calculation
	  if ((phys->isCohesive) && (abs(D) > (Dtensile + Dsoftening))) { // spheres are bonded and the interacting distance is greater than the one allowed ny the defined cohesion
	    phys->isCohesive=false; 
	    // update body state with the number of broken bonds
	    CFpmState* st1=dynamic_cast<CFpmState*>(b1->state.get());
	    CFpmState* st2=dynamic_cast<CFpmState*>(b2->state.get());
	    st1->numBrokenCohesive+=1;
	    st2->numBrokenCohesive+=1;
	    // end of update
	    rootBody->interactions->requestErase(contact->getId1(),contact->getId2()); return;
	  }
	}	  
	
	/*NormalForce*/
	Real Fn=0, Dsoft=0;
		
	if ((D < 0) && (abs(D) > Dtensile)) { //to take into account strength softening
	  Dsoft = D+Dtensile; // Dsoft<0 for a negative value of Fn (attractive force)
	  Fn = -(phys->FnMax+(phys->kn/phys->strengthSoftening)*Dsoft); // computes FnMax - FnSoftening
	}
	else {
	  Fn = phys->kn*D;
	}
	phys->normalForce = Fn*geom->normal;  // NOTE normal is position2-position1 - It is directed from particle1 to particle2
	        
	/*ShearForce*/
	Vector3r& shearForce = phys->shearForce; 
		
	// using scGeom function updateShear(Vector3r& shearForce, Real ks, const Vector3r& prevNormal, const State* rbp1, const State* rbp2, Real dt, bool avoidGranularRatcheting)	
	State* st1 = Body::byId(id1,rootBody)->state.get();
	State* st2 = Body::byId(id2,rootBody)->state.get();
	geom->updateShearForce(shearForce, phys->ks, phys->prevNormal, st1, st2, dt, preventGranularRatcheting);
	
	// needed for the next timestep
	phys->prevNormal = geom->normal;
		
	/* Morh-Coulomb criterion */
	Real maxFs = abs(Fn)*phys->tanFrictionAngle + phys->FsMax;

	if (shearForce.squaredNorm() > maxFs*maxFs){ 
	  shearForce*=maxFs/shearForce.norm(); // to fix the shear force to its yielding value
	}
	
	/* Apply forces */
	Vector3r f = phys->normalForce + phys->shearForce;
	// NOTE applyForceAtContactPoint computes torque induced by normal and shear force too
 	applyForceAtContactPoint(f, geom->contactPoint, contact->getId2(), b2->state->pos, contact->getId1(), b1->state->pos, rootBody); // NOTE id1 and id2 must suit the orientation of the force

	/* Moment Rotation Law */
	// NOTE this part could probably be computed in ScGeom to avoid copy/paste multiplication !!!
	Quaternionr delta( b1->state->ori * phys->initialOrientation1.conjugate() *phys->initialOrientation2 * b2->state->ori.conjugate()); //relative orientation
	AngleAxisr aa(angleAxisFromQuat(delta)); // axis of rotation - this is the Moment direction UNIT vector; angle represents the power of resistant ELASTIC moment
	if(aa.angle() > Mathr::PI) aa.angle() -= Mathr::TWO_PI; // angle is between 0 and 2*pi, but should be between -pi and pi 
	  
	phys->cumulativeRotation = aa.angle();
	  
	//Find angle*axis. That's all.  But first find angle about contact normal. Result is scalar. Axis is contact normal.
	Real angle_twist(aa.angle() * aa.axis().dot(geom->normal) ); //rotation about normal
	Vector3r axis_twist(angle_twist * geom->normal);
	Vector3r moment_twist(axis_twist * phys->kr);
	  
	Vector3r axis_bending(aa.angle()*aa.axis() - axis_twist); //total rotation minus rotation about normal
	Vector3r moment_bending(axis_bending * phys->kr);
	Vector3r moment = moment_twist + moment_bending;

	Real MomentMax = phys->maxBend*std::fabs(phys->normalForce.norm());
	Real scalarMoment = moment.norm();

	/*Plastic moment */
	if(scalarMoment > MomentMax) 
	{
	  Real ratio = 0;
	  ratio *= MomentMax/scalarMoment; // to fix the moment to its yielding value
	  moment *= ratio;		
	  moment_twist *=  ratio;
	  moment_bending *= ratio;
	}

	phys->moment_twist = moment_twist;
	phys->moment_bending = moment_bending;
	  
	rootBody->forces.addTorque(id1,-moment);
	rootBody->forces.addTorque(id2, moment);
		
}

CREATE_LOGGER(Ip2_CFpmMat_CFpmMat_CFpmPhys);

void Ip2_CFpmMat_CFpmMat_CFpmPhys::go(const shared_ptr<Material>& b1, const shared_ptr<Material>& b2, const shared_ptr<Interaction>& interaction){
	
	/* avoid any updates if the interaction already exists */
	if(interaction->interactionPhysics) return; 

	ScGeom* geom=dynamic_cast<ScGeom*>(interaction->interactionGeometry.get());
	assert(geom);

	const shared_ptr<CFpmMat>& yade1 = YADE_PTR_CAST<CFpmMat>(b1);
	const shared_ptr<CFpmMat>& yade2 = YADE_PTR_CAST<CFpmMat>(b2);
			
	shared_ptr<CFpmPhys> contactPhysics(new CFpmPhys()); 
	
	/* From interaction physics */
	Real E1 	= yade1->young;
	Real E2 	= yade2->young;
	Real V1 	= yade1->poisson;
	Real V2 	= yade2->poisson;
	Real f1 	= yade1->frictionAngle;
	Real f2 	= yade2->frictionAngle;

	/* From interaction geometry */
	Real R1= geom->radius1;
	Real R2= geom->radius2;
	Real rMean = 0.5*(R1+R2); 
	Real crossSection = Mathr::PI*pow(min(R1,R2),2); 
	
	/* calculate stiffness */
	Real kNormal=0, kShear=0, kRotate=0;
	
	if(useAlphaBeta == true){
	  kNormal	= 2.*E1*R1*E2*R2/(E1*R1+E2*R2); // harmonic average of two stiffnesses
	  kShear 	= Alpha*kNormal;
	  kRotate	= Beta*kShear*rMean*rMean; 
	}
	else {
	  kNormal	= 2.*E1*R1*E2*R2/(E1*R1+E2*R2); // harmonic average of two stiffnesses
	  kShear	= 2.*E1*R1*V1*E2*R2*V2/(E1*R1*V1+E2*R2*V2); // harmonic average of two stiffnesses with ks=V*kn for each sphere
	  kRotate	= 0.;
	}

	/* Pass values calculated from above to CFpmPhys */
	contactPhysics->kn = kNormal;
	contactPhysics->ks = kShear;
	contactPhysics->kr = kRotate;
	contactPhysics->frictionAngle		= std::min(f1,f2); 
	contactPhysics->tanFrictionAngle	= std::tan(contactPhysics->frictionAngle); 
	contactPhysics->maxBend 		= eta*rMean;
	contactPhysics->prevNormal 		= geom->normal;
	contactPhysics->initialOrientation1	= Body::byId(interaction->getId1())->state->ori;
	contactPhysics->initialOrientation2	= Body::byId(interaction->getId2())->state->ori;
	
	///to set if the contact is cohesive or not
	if ( (scene->currentIteration < cohesiveTresholdIteration) && ((tensileStrength>0) || (cohesion>0)) && (yade1->type == yade2->type)){ contactPhysics->isCohesive=true; }
	
	if ( contactPhysics->isCohesive ) {
	  contactPhysics->FnMax = tensileStrength*crossSection;
	  contactPhysics->strengthSoftening = strengthSoftening;
	  contactPhysics->FsMax = cohesion*crossSection;
	}
	
	interaction->interactionPhysics = contactPhysics;
}

CFpmPhys::~CFpmPhys(){}
