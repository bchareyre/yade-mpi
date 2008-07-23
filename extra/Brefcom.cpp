// 2007,2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include"Brefcom.hpp"
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/lib-QGLViewer/qglviewer.h>


YADE_PLUGIN("BrefcomMakeContact","BrefcomContact","BrefcomLaw","GLDrawBrefcomContact","BrefcomDamageColorizer" /* ,"BrefcomStiffnessCounter"*/ );

/********************** BrefcomMakeContact ****************************/
CREATE_LOGGER(BrefcomMakeContact);


//! @todo Formulas in the following should be verified
void BrefcomMakeContact::go(const shared_ptr<PhysicalParameters>& pp1, const shared_ptr<PhysicalParameters>& pp2, const shared_ptr<Interaction>& interaction){

	const shared_ptr<SpheresContactGeometry>& contGeom=dynamic_pointer_cast<SpheresContactGeometry>(interaction->interactionGeometry);
	assert(contGeom); // for now, don't handle anything other than SpheresContactGeometry

	if(!interaction->isNew && interaction->interactionPhysics){
		/* BrefcomContact->prevNormal is assigned in BrefcomLaw::action */
		#if 0
			const shared_ptr<BrefcomContact> contPhys=dynamic_pointer_cast<BrefcomContact>(interaction->interactionPhysics);
			assert(contPhys);
			contPhys->prevNormal=contGeom->normal;
		#endif
	} else {
		interaction->isNew; // just in case
		//TRACE;


		const shared_ptr<BodyMacroParameters>& elast1=static_pointer_cast<BodyMacroParameters>(pp1);
		const shared_ptr<BodyMacroParameters>& elast2=static_pointer_cast<BodyMacroParameters>(pp2);

		Real E12=2*elast1->young*elast2->young/(elast1->young+elast2->young); // harmonic Young's modulus average
		//Real nu12=2*elast1->poisson*elast2->poisson/(elast1->poisson+elast2->poisson); // dtto for Poisson ratio 
		Real S12=Mathr::PI*pow(min(contGeom->radius1,contGeom->radius2),2); // "surface" of interaction
		//Real d0=contGeom->radius1 + contGeom->radius2; // equilibrium distace is "just touching"
		Real d0=(elast1->se3.position-elast2->se3.position).Length(); // equilibrium distance is the initial contact distance
		//Real E=(E12 /* was here for Kn:  *S12/d0  */)*((1+alpha)/(beta*(1+nu12)+gamma*(1-alpha*nu12)));
		//Real E=E12; // apply alpha, beta, gamma: garbage values of E !?

		/* recommend default values for parameters
		 * propose ways to determine them exactly */
		if(!neverDamage) { assert(!isnan(expBending)); assert(!isnan(sigmaT)); assert(!isnan(xiShear));}

		shared_ptr<BrefcomContact> contPhys(new BrefcomContact(
			/* E */ E12,
			/* G */ E12*G_over_E, /*/2*(1+nu12)*(1-alpha*nu12)/(1+nu12),*/
			/* tanFrictionAngle */ tan(.5*(elast1->frictionAngle+elast2->frictionAngle)),
			/* undamagedCohesion */ sigmaT,
			/* equilibriumDist */ d0,
			/* crossSection */ S12,
			/* epsCrackOnset */ epsCrackOnset,
			/* epsFracture */ relDuctility*epsCrackOnset,
			/* expBending */ expBending,
			/* xiShear*/ xiShear
			));
		contPhys->prevNormal=contGeom->normal;
		if(neverDamage) contPhys->neverDamage=true;
		if(cohesiveThresholdIter<0 || Omega::instance().getCurrentIteration()<cohesiveThresholdIter) contPhys->isCohesive=true;
		else contPhys->isCohesive=false;
		contPhys->tau=tau;
		contPhys->expDmgRate=expDmgRate;

		interaction->interactionPhysics=contPhys;
	}
}




/********************** BrefcomContact ****************************/
CREATE_LOGGER(BrefcomContact);



/********************** BrefcomLaw ****************************/
CREATE_LOGGER(BrefcomLaw);

void BrefcomLaw::applyForce(const Vector3r force){
	Shop::Bex::force(id1,rootBody)+=force;
	Shop::Bex::force(id2,rootBody)-=force;
	Shop::Bex::momentum(id1,rootBody)+=(contGeom->contactPoint-rbp1->se3.position).Cross(force);
	Shop::Bex::momentum(id1,rootBody)-=(contGeom->contactPoint-rbp2->se3.position).Cross(force);
#if 0
	static_pointer_cast<Force>(rootBody->physicalActions->find(id1,ForceClassIndex))->force+=force;
	static_pointer_cast<Force>(rootBody->physicalActions->find(id2,ForceClassIndex))->force-=force;
	static_pointer_cast<Momentum>(rootBody->physicalActions->find(id1,MomentumClassIndex))->momentum+=(contGeom->contactPoint-rbp1->se3.position).Cross(force);
	static_pointer_cast<Momentum>(rootBody->physicalActions->find(id2,MomentumClassIndex))->momentum-=(contGeom->contactPoint-rbp2->se3.position).Cross(force);
#endif
}

void BrefcomLaw::action(MetaBody* _rootBody){
	rootBody=_rootBody;
	
	FOREACH(const shared_ptr<Interaction>& I, *rootBody->transientInteractions){
		if(!I->isReal) continue;
		//TRACE;
		// initialize temporaries
		id1=I->getId1(); id2=I->getId2();
		body1=Body::byId(id1,_rootBody); body2=Body::byId(id2,_rootBody);
		assert(body1); assert(body2);
		BC=YADE_PTR_CAST<BrefcomContact>(I->interactionPhysics);
		contGeom=YADE_PTR_CAST<SpheresContactGeometry>(I->interactionGeometry);
		rbp1=YADE_PTR_CAST<RigidBodyParameters>(body1->physicalParameters);
		rbp2=YADE_PTR_CAST<RigidBodyParameters>(body2->physicalParameters);
		assert(BC); assert(contGeom); assert(rbp1); assert(rbp2);
		
		#ifdef BREFCOM_REC
			recValues.clear(); recLabels.clear();
			BREFREC2(Omega::instance().getCurrentIteration(),"iter");
			BREFREC(id1);
			BREFREC(id2);
			BREFREC2(rbp1->se3.position[0],"x1");
			BREFREC2(rbp1->se3.position[1],"y1");
			BREFREC2(rbp1->se3.position[2],"z1");
			BREFREC2(rbp2->se3.position[0],"x2");
			BREFREC2(rbp2->se3.position[1],"y2");
			BREFREC2(rbp2->se3.position[2],"z2");
			BREFREC2(BC->omega,"ω_old");
		#endif

		// shorthands
		Real& epsN(BC->epsN); Vector3r& epsT(BC->epsT); Real& kappaD(BC->kappaD); const Real& equilibriumDist(BC->equilibriumDist); const Real& xiShear(BC->xiShear); const Real& E(BC->E); const Real& undamagedCohesion(BC->undamagedCohesion); const Real& tanFrictionAngle(BC->tanFrictionAngle); const Real& G(BC->G); const Real& crossSection(BC->crossSection); const Real& tau(BC->tau); const Real& expDmgRate(BC->expDmgRate);
		// for python access
		Real& omega(BC->omega); Real& sigmaN(BC->sigmaN);  Vector3r& sigmaT(BC->sigmaT); Real& Fn(BC->Fn); Vector3r& Fs(BC->Fs);

		Real dist=(rbp1->se3.position-rbp2->se3.position).Length();
		#define NNAN(a) assert(!isnan(a));
		#define NNANV(v) assert(!isnan(v[0])); assert(!isnan(v[1])); assert(!isnan(v[2]));
		assert(equilibriumDist>0);	assert(dist!=0);
		NNAN(dist);

		/*LOG_DEBUG(" ============= ITERATION "<<Omega::instance().getCurrentIteration()<<" ================");
		TRVAR4(epsN,epsT,kappaD,equilibriumDist);
		TRVAR2(rbp1->se3.position,rbp2->se3.position);
		TRVAR2(rbp1->velocity,rbp2->velocity);
		TRVAR2(rbp1->angularVelocity,rbp2->angularVelocity);
		TRVAR3(BC->prevNormal,contGeom->normal,contGeom->contactPoint);
		TRVAR1(Omega::instance().getTimeStep());*/


		/* normal strain */
		epsN=(dist-equilibriumDist)/equilibriumDist;
		NNAN(epsN);

		// /* TODO: recover non-cohesive contact deletion: */
		if(!BC->isCohesive && epsN>0.){ /* delete this interaction later */ I->isReal=false; continue; }
		#define BREFCOM_DETAIL
		/* shear strain: always use it, even for epsN>0 */
		/*if(false && epsN>0) { epsT=Vector3r::ZERO; } else {*/
				NNANV(epsT);
			/* rotate epsT to the new contact plane */
				const Real& dt=Omega::instance().getTimeStep();
				// rotation of the contact normal
				#ifdef BREFCOM_DETAIL
					TRVAR2(epsT,BC->prevNormal.Cross(contGeom->normal));
					TRVAR1((BC->prevNormal.Cross(contGeom->normal)).Cross(epsT));
				#endif
				epsT+=(BC->prevNormal.Cross(contGeom->normal)).Cross(epsT);
				NNANV(epsT);
				
				// mutual rotation
				Real angle=dt*.5*contGeom->normal.Dot(rbp1->angularVelocity+rbp2->angularVelocity); /*assumes equal radii */
				#ifdef BREFCOM_DETAIL
					TRVAR1(epsT);
					TRVAR1(dt*.5*contGeom->normal.Dot(rbp1->angularVelocity+rbp2->angularVelocity));
					TRVAR1((angle*contGeom->normal).Cross(epsT));
				#endif
				epsT+=(angle*contGeom->normal).Cross(epsT);
				NNANV(epsT);

			/* calculate tangential strain increment */
				Vector3r AtoC(contGeom->contactPoint-rbp1->se3.position), BtoC(contGeom->contactPoint-rbp2->se3.position);
				//TRVAR3(contGeom->contactPoint,rbp1->se3.position,rbp2->se3.position);
				Vector3r relVelocity /* at the contact point */ = 
					rbp2->velocity-rbp1->velocity +
					rbp2->angularVelocity.Cross(BtoC)
					-rbp1->angularVelocity.Cross(AtoC);

				Vector3r tangentialDisplacement=dt*(relVelocity- /* subtract non-shear component */ contGeom->normal.Dot(relVelocity)*contGeom->normal);
				#ifdef BREFCOM_DETAIL
					TRVAR1(epsT);
					TRVAR4(AtoC,BtoC,rbp2->angularVelocity,rbp1->angularVelocity);
					TRVAR3(relVelocity,tangentialDisplacement,tangentialDisplacement/dist);
					TRWM3VEC(contGeom->normal);
				#endif
				epsT+=tangentialDisplacement/dist;
				#ifdef BREFCOM_DETAIL
					TRVAR1(epsT);
				#endif
				NNANV(epsT);
			/* artificially remove residuum in the normal direction */
			//epsT-=contGeom->normal*epsT.Dot(contGeom->normal);
			//TRVAR1(epsT.Dot(contGeom->normal));
			
		#ifdef BREFCOM_MATERIAL_MODEL
			BREFCOM_MATERIAL_MODEL
		#else
			sigmaN=E*epsN;
			sigmaT=G*epsT;
		#endif

		//sigmaT=Vector3r::ZERO;
		//if(BC->omega==1){TRVAR5(equilibriumDist,dist,epsN,kappaD,BC->epsFracture);}

		// store Fn (and Fs?), for use with BrefcomStiffnessCounter
		NNAN(sigmaN); NNAN(sigmaT[0]);NNAN(sigmaT[1]);NNAN(sigmaT[2]);
		NNAN(crossSection);
		Fn=sigmaN*crossSection;
		Fs=sigmaT*crossSection;
		//TRVAR5(epsN,epsT,sigmaN,sigmaT,crossSection*(contGeom->normal*sigmaN + sigmaT));
		applyForce(crossSection*(contGeom->normal*sigmaN + sigmaT)); /* this is the force applied on the _first_ body */

		#ifdef BREFCOM_REC
			/*BREFREC(epsN); BREFREC(epsT[0]);BREFREC(epsT[1]);BREFREC(epsT[2]);*/
			BREFREC(epsN); BREFREC(Fn);
			BREFREC2(BC->omega,"ω_new");
			for(size_t i=0; i<recValues.size(); i++) recStream<<recLabels[i]<<": "<<recValues[i]<<" ";
			recStream<<endl;
		#endif

		/* store this normal for next timestep */
		BC->prevNormal=contGeom->normal;
	}
}


/********************** GLDrawBrefcomContact ****************************/

#include<yade/lib-opengl/OpenGLWrapper.hpp>

CREATE_LOGGER(GLDrawBrefcomContact);

bool GLDrawBrefcomContact::contactLine=true;
bool GLDrawBrefcomContact::dmgLabel=true;
bool GLDrawBrefcomContact::epsT=false;
bool GLDrawBrefcomContact::epsTAxes=false;
bool GLDrawBrefcomContact::normal=false;


void GLDrawBrefcomContact::go(const shared_ptr<InteractionPhysics>& ip, const shared_ptr<Interaction>& i, const shared_ptr<Body>& b1, const shared_ptr<Body>& b2, bool wireFrame){
	const shared_ptr<BrefcomContact>& BC=static_pointer_cast<BrefcomContact>(ip);

	/* shared_ptr<Sphere> s1(dynamic_pointer_cast<Sphere>(b1->geometricalModel)), s2(dynamic_pointer_cast<Sphere>(b2->geometricalModel));
	if(s1 && s2){
		Real radius=min(s1->radius,s2->radius), len=(b1->physicalParameters->se3.position-b2->physicalParameters->se3.position).Length();
		glScale(len,radius,radius);
		glutSolidCube(1);
	} */

	if(contactLine) Shop::GLDrawLine(b1->physicalParameters->dispSe3.position,b2->physicalParameters->dispSe3.position,Vector3r(BC->omega,1-BC->omega,0.0) /* damaged links red, undamaged green */ );
	if(dmgLabel){ Shop::GLDrawNum(BC->omega,0.5*(b1->physicalParameters->dispSe3.position+b2->physicalParameters->dispSe3.position),Vector3r(BC->omega,1-BC->omega,0.)); }
	const Vector3r& cp=static_pointer_cast<SpheresContactGeometry>(i->interactionGeometry)->contactPoint;
	if(epsT){
		Real maxShear=(BC->undamagedCohesion-BC->sigmaN*BC->tanFrictionAngle)/BC->G;
		Real relShear=BC->epsT.Length()/maxShear;
		Real scale=.5*BC->equilibriumDist;
		Vector3r dirShear=BC->epsT; dirShear.Normalize();
		if(epsTAxes){
			Shop::GLDrawLine(cp-Vector3r(scale,0,0),cp+Vector3r(scale,0,0));
			Shop::GLDrawLine(cp-Vector3r(0,scale,0),cp+Vector3r(0,scale,0));
			Shop::GLDrawLine(cp-Vector3r(0,0,scale),cp+Vector3r(0,0,scale));
		}
		Shop::GLDrawArrow(cp,cp+dirShear*relShear*scale,Vector3r(1.,0.,0.));
		Shop::GLDrawLine(cp+dirShear*relShear*scale,cp+dirShear*scale,Vector3r(.3,.3,.3));

		/* normal strain */ Shop::GLDrawArrow(cp,cp+BC->prevNormal*(BC->epsN/maxShear),Vector3r(0.,1.,0.));
	}
	//if(normal) Shop::GLDrawArrow(cp,cp+BC->prevNormal*.5*BC->equilibriumDist,Vector3r(0.,1.,0.));
}

/********************** BrefcomDamageColorizer ****************************/
void BrefcomDamageColorizer::action(MetaBody* rootBody){
	vector<pair<short,Real> > bodyDamage; /* number of cohesive interactions per body; cummulative damage of interactions */
	bodyDamage.resize(rootBody->bodies->size(),pair<short,Real>(0,0));
	FOREACH(shared_ptr<Interaction> I, *rootBody->transientInteractions){
		shared_ptr<BrefcomContact> BC=dynamic_pointer_cast<BrefcomContact>(I->interactionPhysics);
		if(!BC || !BC->isCohesive) continue;
		const body_id_t id1=I->getId1(), id2=I->getId2();
		bodyDamage[id1].first++; bodyDamage[id2].first++;
		bodyDamage[id1].second+=BC->omega; bodyDamage[id2].second+=BC->omega;
	}
	FOREACH(shared_ptr<Body> B, *rootBody->bodies){
		if(bodyDamage[B->getId()].first==0) {B->geometricalModel->diffuseColor=Vector3r(0.5,0.5,B->isDynamic?0:1); continue; }
		Real normDmg=bodyDamage[B->getId()].second/bodyDamage[B->getId()].first;
		B->geometricalModel->diffuseColor=Vector3r(normDmg,1-normDmg,B->isDynamic?0:1);
	}
}

/* doesn't work correctly */
#if 0
void BrefcomStiffnessCounter::action(MetaBody* rootBody){
	InteractionContainer::iterator Iend=rootBody->transientInteractions->end();
	for(InteractionContainer::iterator I=rootBody->transientInteractions->begin(); I!=Iend; ++I){
		const shared_ptr<Interaction>& contact = *I;
		if(!contact->isReal) continue;
		shared_ptr<SpheresContactGeometry> cGeom=YADE_PTR_CAST<SpheresContactGeometry>(contact->interactionGeometry);
		shared_ptr<BrefcomContact> cPhys=YADE_PTR_CAST<BrefcomContact>(contact->interactionPhysics);
		if(cPhys->Fn==0) continue;
		Vector3r diag_stiffness=Vector3r(pow(cGeom->normal[0],2),pow(cGeom->normal[1],2),pow(cGeom->normal[2],2))*(cPhys->Kn-cPhys->Kt)+Vector3r(1,1,1)*cPhys->Kt;
		Vector3r diag_Rstiffness=Vector3r(pow(cGeom->normal[1],2)+pow(cGeom->normal[2],2),pow(cGeom->normal[0],2)+pow(cGeom->normal[2],2),pow(cGeom->normal[0],2)+pow(cGeom->normal[1],2))*cPhys->Kt;
		GlobalStiffness* s=static_cast<GlobalStiffness*>(rootBody->physicalActions->find(contact->getId1(),actionStiffnessIndex).get());
		s->stiffness+=diag_stiffness; s->Rstiffness+=diag_Rstiffness*pow(cGeom->radius1,2);	
		s=static_cast<GlobalStiffness*>(rootBody->physicalActions->find(contact->getId2(),actionStiffnessIndex).get());
		s->stiffness+=diag_stiffness; s->Rstiffness+=diag_Rstiffness*pow(cGeom->radius2,2);
	}
}
#endif 


/*****************************************************************************************************
 ********************* static versions of damage evolution law and fracture energy integration *******
 *****************************************************************************************************/

#if 0
/*! Damage evolution law (static version: all parameters are explicitly passed).
 *
 * This function is zero for any eps<=epsCrackOnset (no damage before the material starts cracking),
 * therefore it will be linear-elastic. Between epsCrackOnset and espFracture (complete damage), it
 * is a non-decreasing function (an exp curve in this case, controlled by a single parameter expBending).
 * For any exp>=expFracture, return 1 (complete damage).
 *
 * @param kappaD maximum positive strain so far
 * @param expBending determines whether the function is bent up or down (and how much)
 * @param epsCrackOnsert strain at which material begins to behave non-linearly
 * @param epsFracture strain at which material is fully damaged
 */
Real BrefcomLaw::damageEvolutionLaw_static(Real kappaD, Real expBending, Real epsCrackOnset, Real epsFracture){
	//double g(double x, double c, double eps0, double eps1){
	if(kappaD<epsCrackOnset)return 0;
	if(kappaD>epsFracture)return 1;
	return (1/(1-exp(-expBending)))*(1-exp(-expBending*(kappaD-epsCrackOnset)/(epsFracture-epsCrackOnset)));
}
/*! Compute fracture energy by numerically integrating damageEvolutionLaw_static, using unitary stiffness.
 *
 * The value returned must be multiplied by E to obtain real fracture energy: Fracture energy
 *    Gf=integral(E*(1-damage(eps))*eps)=E*integral(...) and we compute just the integral(...) part.
 * The integration uses trapezoidal rule. All parameters except steps have the same
 * meaning as for damageEvolutionLaw_static.
 *
 * @param steps Number of subdivision intervals for integration.
 */

Real BrefcomLaw::unitFractureEnergy(double expBending, double epsCrackOnset, double epsFracture, int steps /*=50*/){
	assert(steps>=10);
	const double lo=0, hi=epsFracture;
	double sum=0,stepSize=(hi-lo)/steps,x1,x2;
	for(int i=0; i<steps; i++){
		x1=lo+i*stepSize; x2=x1+stepSize;
		sum+=((1-damageEvolutionLaw_static(x1,expBending,epsCrackOnset,epsFracture))*x1+(1-damageEvolutionLaw_static(x2,expBending,epsCrackOnset,epsFracture))*x2)*.5*stepSize; /* trapezoid rule */
	}
	return sum;
}

/*! Calibrate epsFracture so that the desired fracture energy is obtained, while other parameters are keps constant.
 *
 * The iteration relies on the fact that fractureEnergy is increasing for increasing epsFracture;
 * first we find some strain value eps at which fractureEnergy(eps,...)<Gf<fractureEnergy(2*eps,...),
 * then the interval is bisected until fractureEnergy matches Gf with desired precision.
 *
 * @param Gf the desired fracture energy.
 * @param relEpsilon relative (with regards to Gf) result precision.
 * @param maxIter number of iterations after which we throw exception, since for a reason we don't converge or converge too slowly.
 *
 * Other params have the same meaning as for damageEvolutionLaw_static.
 */
Real BrefcomLaw::calibrateEpsFracture(double Gf, double E, double expBending, double epsCrackOnset, double epsFractureInit /*=1e-3*/, double relEpsilon /*=1e-3*/, int maxIter /*=1000*/){
	double E1,E2,Emid,epsLo=epsFractureInit,epsHi=epsFractureInit*2,epsMid,iter=0,Gf_div_E=Gf/E;
	bool goUp=unitFractureEnergy(expBending,epsCrackOnset,epsHi)<Gf_div_E; // do we double up or down when finding margins?
	do { epsLo*=goUp?2:.5; epsHi=2*epsLo;
		E1=unitFractureEnergy(expBending,epsCrackOnset,epsLo); E2=unitFractureEnergy(expBending,epsCrackOnset,epsHi);
		if((iter++)>maxIter) throw runtime_error("Convergence problem when finding margin values for bisection.");
	} while(!(E1<Gf_div_E && E2>=Gf_div_E));
	// now E(epsLo)<Gf_div_E<=E(epsHi); go ahead using interval bisection
	do {
		epsMid=.5*(epsLo+epsHi); Emid=unitFractureEnergy(expBending,epsCrackOnset,epsMid);
		if(Emid<Gf_div_E)epsLo=.5*(epsLo+epsHi);
		else if(Emid>Gf_div_E)epsHi=.5*(epsLo+epsHi);
		if((iter++)>maxIter) throw runtime_error("Convergence problem during bisection (relEpsilon too low?).");
	} while (abs(Emid-Gf_div_E)>relEpsilon*Gf_div_E);
	return epsMid;
}
#endif

