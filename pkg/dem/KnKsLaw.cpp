#ifdef YADE_POTENTIAL_PARTICLES
#include "KnKsLaw.hpp"
#include <core/Scene.hpp>
#include <pkg/dem/ScGeom.hpp>
#include <core/Omega.hpp>
#include <pkg/dem/PotentialParticle.hpp>

YADE_PLUGIN((Law2_SCG_KnKsPhys_KnKsLaw)(Ip2_FrictMat_FrictMat_KnKsPhys)(KnKsPhys)

           );



/********************** Law2_Dem3DofGeom_RockPMPhys_Rpm ****************************/
CREATE_LOGGER(Law2_SCG_KnKsPhys_KnKsLaw);



bool Law2_SCG_KnKsPhys_KnKsLaw::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact) {
	//const Real& dt = scene->dt;
	int id1 = contact->getId1();
	int id2 = contact->getId2();
	ScGeom*    geom= static_cast<ScGeom*>(ig.get());
	KnKsPhys* phys = static_cast<KnKsPhys*>(ip.get());
	State* de1 = Body::byId(id1,scene)->state.get();
	State* de2 = Body::byId(id2,scene)->state.get();
	Shape* shape1 = Body::byId(id1,scene)->shape.get();
	Shape* shape2 = Body::byId(id2,scene)->shape.get();
	PotentialParticle *s1=static_cast<PotentialParticle*>(shape1);
	PotentialParticle *s2=static_cast<PotentialParticle*>(shape2);
	Vector3r& shearForce = phys->shearForce;
	Real un=geom->penetrationDepth;
	TRVAR3(geom->penetrationDepth,de1->se3.position,de2->se3.position);

	/* Need to initialise in python.  In the 1st time step.  All the particles in contact (controlled by initialOverlap) are identified.  The interactions are set to tensile and cohesive (tensionBroken = false and cohesionBroken = false).  If there is no initial tension or cohesion, the contact law is run in a tensionless or cohesionless mode */

	if(geom->penetrationDepth <0.0 ) {
		if (neverErase) {
			phys->shearForce = Vector3r::Zero();
			phys->normalForce = Vector3r::Zero();
			phys->normalViscous = Vector3r::Zero();
			geom->normal = Vector3r::Zero();
			phys->tensionBroken = true;
		} else {
			scene->interactions->requestErase(id1,id2);
			return false;
		}
		return true;
	}

	//Vector3r shearForceBeforeRotate = shearForce;
	Vector3r shiftVel = Vector3r(0,0,0); //scene->isPeriodic ? (Vector3r)((scene->cell->velGrad*scene->cell->Hsize)*Vector3r((Real) contact->cellDist[0],(Real) contact->cellDist[1],(Real) contact->cellDist[2])) : Vector3r::Zero();
	geom->rotate(shearForce); //AndGetShear(shearForce,phys->prevNormal,de1,de2,dt,shiftVel,/*avoid ratcheting*/false);
	//Vector3r shearForceAfterRotate = shearForce;
	//Linear elasticity giving "trial" shear force
	Vector3r shift2(0,0,0);
	Vector3r incidentV = geom->getIncidentVel(de1, de2, scene->dt, shift2, shiftVel, /*preventGranularRatcheting*/false );
	Vector3r incidentVn = geom->normal.dot(incidentV)*geom->normal; // contact normal velocity
	Vector3r incidentVs = incidentV-incidentVn; // contact shear velocity
	Vector3r shearIncrement=incidentVs*scene->dt;
	phys->shearDir = shearIncrement;
	phys->shearIncrementForCD += shearIncrement.norm();
	double du = 0.0;
	//double debugFn = 0.0;
	//double u_prev = fabs(phys->u_cumulative);
	if(phys->shearDir.norm() > pow(10,-15)) {
		phys->shearDir.normalize();
	}
	double degradeLength = phys->brittleLength;  /*jointLength = 100u_peak */
	/* Elastic and plastic displacement can have negative signs but must be consistent throughout the simulation */
	if(phys->initialShearDir.norm() < pow(10,-11)) {
		phys->initialShearDir = phys->shearDir;
		du = shearIncrement.norm();
		if(fabs(phys->mobilizedShear)>0.99999) {
			phys->u_cumulative += du;
			phys->cumulative_us += du;
		} else {
			phys->u_elastic +=du;
		}
	} else {
		du = Mathr::Sign(phys->initialShearDir.dot(phys->shearDir))*shearIncrement.norm(); //check cumulative shear displacement
		if(fabs(phys->mobilizedShear) > 0.99999) {
			if(du>0.0) { //if negative it means it is unloading
				phys->u_cumulative += du;
				phys->cumulative_us += du;
			} else {
				phys->u_elastic +=du;
			}
		} else {
			phys->u_elastic +=du;
		}
	}


	/* Original */
	if(phys->twoDimension) {
		phys->contactArea = phys->unitWidth2D*phys->jointLength;
	}
	if(s1->isBoundary == true || s2->isBoundary==true) {
		phys->tensionBroken = true;
		phys->cohesionBroken = true;
	}
	if(!Talesnick) {
		un = un-initialOverlapDistance;

		if (phys->jointType==3) {
			phys->prevSigma  = un*phys->kn_i/(1.0-un/phys->maxClosure);
		} else {
			phys->prevSigma = phys->kn*un;
		}
		//}
		phys->normalForce = phys->prevSigma*std::max(pow(10,-15),phys->contactArea)*geom->normal;
	}

	phys->Knormal_area = phys->kn*std::max(pow(10,-8),phys->contactArea);

	if((un <0.0 && fabs(phys->prevSigma)>phys->tension && phys->tensionBroken == false /* first time tension is broken */) || (un<0.0 && phys->tensionBroken==true)) {
		if (neverErase) {
			phys->shearForce = Vector3r::Zero();
			phys->normalForce = Vector3r::Zero();
			phys->normalViscous = Vector3r::Zero();
			geom->normal = Vector3r::Zero();
			phys->tensionBroken = true;
		} else 	{
			return false;
		}
		return true;
	}




	/*ORIGINAL */
	Vector3r c1x = geom->contactPoint - de1->pos;
	Vector3r c2x = geom->contactPoint - de2->pos;
	incidentV = (de2->vel+de2->angVel.cross(c2x)) - (de1->vel+de1->angVel.cross(c1x));
	incidentVn = geom->normal.dot(incidentV)*geom->normal; // contact normal velocity
	incidentVs = incidentV-incidentVn; // contact shear velocity
	shearIncrement=incidentVs*scene->dt;
	if(!Talesnick) {
		double Ks=0.0;
		if(phys->jointType == 3) {
			Ks = phys->ks_i*pow(phys->prevSigma,0.6);
		} else {
			Ks = phys->ks;
		}
		shearForce -= Ks*shearIncrement*std::max(pow(10,-11),phys->contactArea);
	}
	phys->Kshear_area = phys->ks*std::max(pow(10,-11),phys->contactArea);


	const shared_ptr<Body>& b1=Body::byId(id1,scene);
	const shared_ptr<Body>& b2=Body::byId(id2,scene);
	Real mbar = (!b1->isDynamic() && b2->isDynamic()) ? de2->mass : ((!b2->isDynamic() && b1->isDynamic()) ? de1->mass : (de1->mass*de2->mass / (de1->mass + de2->mass))); // get equivalent mass if both bodies are dynamic, if not set it equal to the one of the dynamic body
	Real Cn_crit = 2.*sqrt(mbar*phys->Knormal_area); // Knormal_area Critical damping coefficient (normal direction)
	Real Cs_crit = 2.*sqrt(mbar*phys->Kshear_area); // Kshear_area Critical damping coefficient (shear direction)
	// Note: to compare with the analytical solution you provide cn and cs directly (since here we used a different method to define c_crit)
	double cn = Cn_crit*phys->viscousDamping; // Damping normal coefficient
	double cs = Cs_crit*phys->viscousDamping; // Damping tangential coefficient

	// add normal viscous component if damping is included
	//double maxFnViscous = phys->normalForce.norm();
	phys->normalViscous = cn*incidentVn;
	//if(phys->normalViscous.norm() > maxFnViscous){
	//	phys->normalViscous = phys->normalViscous * maxFnViscous/phys->normalViscous.norm();
	//}
	phys->normalForce -= phys->normalViscous;
	//double baseElevation =  geom->contactPoint.z();

	/* Water pressure, heat effect */

	/* strength degradation */
	const double PI = std::atan(1.0)*4;
	double tan_effective_phi = 0.0;



	if(s1->isBoundary==true || s2->isBoundary == true || phys->jointType==2 ) { // clay layer at boundary;
		phys->effective_phi = phys->phi_b; // - 3.25*(1.0-exp(-fabs(phys->cumulative_us)/0.4));
		tan_effective_phi = tan(phys->effective_phi/180.0*PI);
	} else if(phys->intactRock == true) {

		phys->effective_phi = phys->phi_r +  (phys->phi_b-phys->phi_r)*(exp(-fabs(phys->u_cumulative)/degradeLength));
		tan_effective_phi = tan(phys->effective_phi/180.0*PI);
	} else {
		phys->effective_phi = phys->phi_b;
		tan_effective_phi = tan(phys->effective_phi/180.0*PI);
	}



	/* shear loss */
	Vector3r dampedShearForce = shearForce;
	double cohesiveForce = phys->cohesion*std::max(pow(10,-11),phys->contactArea);
	Real maxFs = cohesiveForce;
	if (un>0.0 /*compression*/) {
		double fN = phys->normalForce.norm();
		if(phys->intactRock == true) {
			if (phys->cohesionBroken == true && allowBreakage == true) {
				maxFs = std::max( fN,0.0)*tan_effective_phi;
			} else {
				maxFs = cohesiveForce+std::max( fN,0.0)*tan_effective_phi;
			}
		} else {
			maxFs = std::max( fN,0.0)*tan_effective_phi;
		}
	}
	if( shearForce.norm() > maxFs ) {
		Real ratio = maxFs / shearForce.norm();
		shearForce *= ratio;
		dampedShearForce = shearForce;
		if(allowBreakage == true) {
			phys->cohesionBroken = true;
		}
		phys->shearViscous = Vector3r(0,0,0);
	} else { /* no damping when it slides */
		phys->shearViscous = cs*incidentVs;
		dampedShearForce = shearForce - phys->shearViscous;
	}
	if(shearForce.norm() < pow(10,-11) ) {
		phys->mobilizedShear = 1.0;
	}
	else {
		phys->mobilizedShear = shearForce.norm()/maxFs;
	}



	//we need to use correct branches in the periodic case, the following apply for spheres only
	Vector3r force = -phys->normalForce-dampedShearForce;
	if(std::isnan(force.norm())) {
		//std::cout<<"shearForce: "<<shearForce<<", normalForce: "<<phys->normalForce<<", debugFn: "<<debugFn<<", viscous: "<<phys->normalViscous<<", normal: "<<phys->normal<<", geom normal: "<<geom->normal<<", effective_phi: "<<phys->effective_phi<<", shearIncrement: "<<shearIncrement<<", id1: "<<id1<<", id2: "<<id2<<", shearForceBeforeRotate: "<<shearForceBeforeRotate<<", shearForceAfterRotate: " <<shearForceAfterRotate<<endl;
	}
	scene->forces.addForce(id1,force);
	scene->forces.addForce(id2,-force);
	//Vector3r normal = geom->normal;
	scene->forces.addTorque(id1,c1x.cross(force));
	scene->forces.addTorque(id2,-(c2x).cross(force));

	phys->prevNormal = geom->normal;

	return true;

}



CREATE_LOGGER(Ip2_FrictMat_FrictMat_KnKsPhys);




void Ip2_FrictMat_FrictMat_KnKsPhys::go(const shared_ptr<Material>& b1, const shared_ptr<Material>& b2, const shared_ptr<Interaction>& interaction) {

	const double PI = 3.14159265358979323846;
	if(interaction->phys) return;

	ScGeom* scg=YADE_CAST<ScGeom*>(interaction->geom.get());

	assert(scg);

	const shared_ptr<FrictMat>& sdec1 = YADE_PTR_CAST<FrictMat>(b1);
	const shared_ptr<FrictMat>& sdec2 = YADE_PTR_CAST<FrictMat>(b2);

	shared_ptr<KnKsPhys> contactPhysics(new KnKsPhys());
	//interaction->interactionPhysics = shared_ptr<MomentPhys>(new MomentPhys());
	//const shared_ptr<MomentPhys>& contactPhysics = YADE_PTR_CAST<MomentPhys>(interaction->interactionPhysics);

	/* From interaction physics */
	Real fa 	= sdec1->frictionAngle;
	Real fb 	= sdec2->frictionAngle;


	/* calculate stiffness */
	Real Kn= Knormal;
	Real Ks= Kshear;

	/* Pass values calculated from above to CSPhys */
	contactPhysics->viscousDamping = viscousDamping;
	contactPhysics->useOverlapVol = useOverlapVol;
	contactPhysics->kn = Kn;
	contactPhysics->ks = Ks;
	contactPhysics->kn_i = Kn;
	contactPhysics->ks_i = Ks;
	contactPhysics->u_peak = u_peak;
	contactPhysics->maxClosure = maxClosure;
	contactPhysics->cohesionBroken = cohesionBroken;
	contactPhysics->tensionBroken = tensionBroken;
	contactPhysics->unitWidth2D = unitWidth2D;
	contactPhysics->frictionAngle		= std::min(fa,fb);
	if(!useFaceProperties) {
		contactPhysics->phi_r = std::min(fa,fb)/PI*180.0;
		contactPhysics->phi_b = contactPhysics->phi_r;
	}
	contactPhysics->tanFrictionAngle	= std::tan(contactPhysics->frictionAngle);
	//contactPhysics->initialOrientation1	= Body::byId(interaction->getId1())->state->ori;
	//contactPhysics->initialOrientation2	= Body::byId(interaction->getId2())->state->ori;
	contactPhysics->prevNormal 		= scg->normal; //This is also done in the Contact Law.  It is not redundant because this class is only called ONCE!
	contactPhysics->calJointLength = calJointLength;
	contactPhysics->twoDimension = twoDimension;
	contactPhysics->useFaceProperties = useFaceProperties;
	contactPhysics->brittleLength = brittleLength;
	interaction->phys = contactPhysics;

}

CREATE_LOGGER(KnKsPhys);
/* KnKsPhys */
KnKsPhys::~KnKsPhys() {}

#endif // YADE_POTENTIAL_PARTICLES
