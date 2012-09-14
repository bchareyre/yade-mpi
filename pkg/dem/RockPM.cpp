#include"RockPM.hpp"
#include<yade/core/Scene.hpp>
#include<yade/pkg/dem/DemXDofGeom.hpp>
#include<yade/pkg/dem/Shop.hpp>
#include<math.h>

YADE_PLUGIN((RpmState)(Law2_Dem3DofGeom_RockPMPhys_Rpm)(RpmMat)(Ip2_RpmMat_RpmMat_RpmPhys)(RpmPhys));


/********************** Law2_Dem3DofGeom_RockPMPhys_Rpm ****************************/
CREATE_LOGGER(Law2_Dem3DofGeom_RockPMPhys_Rpm);

void Law2_Dem3DofGeom_RockPMPhys_Rpm::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact){
	Dem3DofGeom* geom=static_cast<Dem3DofGeom*>(ig.get());
	RpmPhys* phys=static_cast<RpmPhys*>(ip.get());
	const BodyContainer& bodies = *scene->bodies;
	
	const Real& crossSection=phys->crossSection;
	
	Real& epsN=phys->epsN;
	Vector3r& epsT=phys->epsT;
	
	Real sigmaN=phys->sigmaN;
	Vector3r sigmaT=phys->sigmaT;
	
	epsN=geom->strainN();
	epsT=geom->strainT();
	
	const Real& E = phys->E;
	const Real& G = phys->G;
	
	Real& Fn=phys->Fn;
	Vector3r& Fs=phys->Fs;
	
	sigmaN=E*epsN;
	sigmaT=G*epsT;
	
	const int id1 = contact->getId1();
	const int id2 = contact->getId2();
	
	const State& de1 = *static_cast<State*>(bodies[id1]->state.get());
	const State& de2 = *static_cast<State*>(bodies[id2]->state.get());
	
	const Vector3r c1x = (geom->contactPoint - de1.pos);
	const Vector3r c2x = (geom->contactPoint - de2.pos);
	
	const Vector3r relativeVelocity = (de1.vel+de1.angVel.cross(c1x)) - (de2.vel+de2.angVel.cross(c2x)) ;
	const Real normalVelocity	= geom->normal.dot(relativeVelocity);
	const Vector3r shearVelocity	= relativeVelocity-normalVelocity*geom->normal;
	
	if (phys->isCohesive) {
		if (epsN<-phys->epsMaxCompression) {
			//std::cout<<"Destruction from Compression\n";
			phys->isCohesive = false;
		} else if (epsN>phys->epsMaxTension) {
			//std::cout<<"Destruction from Tension\n";
			phys->isCohesive = false;
		} else if (epsT.squaredNorm()>(phys->epsMaxShear*phys->epsMaxShear)) {
			//std::cout<<"Destruction from Shear\n";
			phys->isCohesive = false;
		}
	}

	
	if (phys->isCohesive) {					//Cohesive State
		Fn=sigmaN*crossSection; phys->normalForce=Fn*geom->normal-phys->cn*normalVelocity*geom->normal;
		Fs=sigmaT*crossSection; phys->shearForce = Fs-phys->cs*shearVelocity;
	} else {												//UnCohesive State
		if (epsN<0) {															//Compression
			Fn=sigmaN*crossSection;
			phys->normalForce=Fn*geom->normal-phys->cn*normalVelocity*geom->normal;		//Normal Forces
			
			Fs=sigmaT*crossSection;
			Real maxFs = phys->normalForce.norm()*phys->tanFrictionAngle; /**Check for maximal possible friction force**/
			if(Fs.norm()>maxFs) {	Fs*=maxFs/Fs.norm();}
			
			phys->shearForce = Fs-phys->cs*shearVelocity;
			
		} else if (epsN>0) {		//If UnCohesive State and particles are too far from each other, delete the interaction
			scene->interactions->requestErase(id1,id2);
			return;
		}
	}
	
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~Destruction Experimental~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	#if 0
	//EXPERIMENTAL!!!
	if (phys->isCohesive) {
		if ((epsN<0) && (-epsN/phys->epsMaxCompression + epsT.norm()/phys->epsMaxShear) > 1) { phys->isCohesive = false;}
		else if ((epsN>0) && (epsN/phys->epsMaxTension + epsT.norm()/phys->epsMaxShear)  > 1) { phys->isCohesive = false;}
	}
	#endif
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~Destruction Experimental~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	#if 0
	sigmaT=G*epsT;
	Real maxFsSq = phys->normalForce.squaredNorm()*phys->tanFrictionAngle*phys->tanFrictionAngle; /**Check for maximal possible friction force**/
	if(sigmaT.squaredNorm()>maxFsSq) {
		sigmaT*=sqrt(maxFsSq/(sigmaT.squaredNorm()));
	}
	Fs=sigmaT*crossSection;
	phys->shearForce = Fs;
	#endif
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	
	applyForceAtContactPoint(phys->normalForce + phys->shearForce, geom->contactPoint, contact->getId1(), geom->se31.position, contact->getId2(), geom->se32.position);
}


CREATE_LOGGER(Ip2_RpmMat_RpmMat_RpmPhys);

void Ip2_RpmMat_RpmMat_RpmPhys::go(const shared_ptr<Material>& pp1, const shared_ptr<Material>& pp2, const shared_ptr<Interaction>& interaction){
	if(interaction->phys) return; 

	Dem3DofGeom* contGeom=YADE_CAST<Dem3DofGeom*>(interaction->geom.get());
	assert(contGeom);
	//LOG_WARN(Omega::instance().getCurrentIteration());
	const shared_ptr<RpmMat>& rpm1=YADE_PTR_CAST<RpmMat>(pp1);
	const shared_ptr<RpmMat>& rpm2=YADE_PTR_CAST<RpmMat>(pp2);
	
	const int id1 = interaction->getId1();
	const int id2 = interaction->getId2();
	
	Body* b1=Body::byId(id1,scene).get();
	Body* b2=Body::byId(id2,scene).get();
	
	long cohesiveThresholdIter=2;
	
	bool initCohesive = rpm1->initCohesive*rpm2->initCohesive;
	
	Real& E1 	= rpm1->young;
	Real& E2 	= rpm2->young;
	Real& V1 	= rpm1->poisson;
	Real& V2 	= rpm2->poisson;
	Real& R1 	=	contGeom->refR1;
	Real& R2 	=	contGeom->refR2;
	
	Real E12=2*E1*E2/(E1+E2);
	Real minRad=(R1<=0?R2:(R2<=0?R1:min(R1,R2)));
	Real S12=Mathr::PI*pow(minRad,2);
	Real G_over_E = (rpm1->G_over_E + rpm2->G_over_E)/2;
	
	
	shared_ptr<RpmPhys> contPhys(new RpmPhys());
	contPhys->E=E12;
	contPhys->G=E12*G_over_E;
	
	if ((rpm1->frictionAngle==0) or (rpm2->frictionAngle==0)) {	//If one of frictionAngle==0, we have no friction
		contPhys->tanFrictionAngle = 0;
	} else {
		contPhys->tanFrictionAngle=tan(.5*(rpm1->frictionAngle+rpm2->frictionAngle));
	}
	
	contPhys->crossSection=S12;
	
	//We take the minimal strength parameter
	contPhys->epsMaxCompression=(std::min(rpm1->stressCompressMax,rpm2->stressCompressMax))/contPhys->E;
	contPhys->epsMaxTension=(std::min(rpm1->stressStretchingMax,rpm2->stressStretchingMax))/contPhys->E;
	contPhys->epsMaxShear=(std::min(rpm1->stressShearMax,rpm2->stressShearMax))/contPhys->G;
	
	Real RR1 = R1; Real RR2 = R2;
	
	if ((RR1=-1)&&(RR2!=-1)) {
		RR1 = RR2;
	} else if ((RR1!=-1)&&(RR2!=-1)) {
		RR2 = RR1;
	}
	contPhys->Kn 	= 2*E1*RR1*E2*RR2/(E1*RR1+E2*RR2);
	contPhys->Ks 	= 2*E1*RR1*V1*E2*RR2*V2/(E1*RR1*V1+E2*RR2*V2);
	
	Real Zeta = 0;
	if ((rpm1->Zeta!=0)&&(rpm2->Zeta!=0)) { Zeta=(rpm1->Zeta + rpm2->Zeta) / 2.0; }
	if ((rpm1->Zeta!=0)&&(rpm2->Zeta==0)) { Zeta=rpm1->Zeta; }
	if ((rpm1->Zeta==0)&&(rpm2->Zeta!=0)) { Zeta=rpm2->Zeta; }
	if ((rpm1->Zeta==0)&&(rpm2->Zeta==0)) { Zeta=0; }
	
	Real m = 0; 																		//Average mass
	if ((b1->state->mass)&&(b2->state->mass)) {
		m = 0.5*(b1->state->mass+b2->state->mass);
	} else if ((b1->state->mass)&&(!b2->state->mass)) {
		m = b1->state->mass;
	} else if ((!b1->state->mass)&&(b2->state->mass)) {
		m = b2->state->mass;
	}
	
	
	contPhys->cn = 2 * sqrt(contPhys->Kn*m)*Zeta;		//Damping coefficient
	//contPhys->cs = contPhys->cn/100.0;
	contPhys->cs = 0.0;
	
	
	if ((rpm1->exampleNumber==rpm2->exampleNumber)&&(initCohesive)&&(scene->iter<=cohesiveThresholdIter)) {
		contPhys->isCohesive=true;
	}

	interaction->phys=contPhys;
}

RpmPhys::~RpmPhys(){};
