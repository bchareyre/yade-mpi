// 2007,2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include"Brefcom.hpp"
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/lib-QGLViewer/qglviewer.h>
#include<yade/lib-opengl/GLUtils.hpp>


YADE_PLUGIN("BrefcomMakeContact","BrefcomContact","BrefcomLaw","GLDrawBrefcomContact","BrefcomDamageColorizer", "BrefcomPhysParams", "BrefcomGlobalCharacteristics", "ef2_Spheres_Brefcom_BrefcomLaw" /* ,"BrefcomStiffnessComputer"*/ );

CREATE_LOGGER(BrefcomGlobalCharacteristics);

void BrefcomGlobalCharacteristics::compute(MetaBody* rb, bool useMaxForce){
	rb->bex.sync();

	// 1. reset volumetric strain (cummulative in the next loop)
	// 2. get maximum force on a body and sum of all forces (for averaging)
	Real sumF=0,maxF=0,currF;
	FOREACH(const shared_ptr<Body>& b, *rb->bodies){
	BrefcomPhysParams* bpp(YADE_CAST<BrefcomPhysParams*>(b->physicalParameters.get()));
		bpp->epsVolumetric=0;
		bpp->numContacts=0;
		currF=rb->bex.getForce(b->id).Length(); maxF=max(currF,maxF); sumF+=currF;
	}
	Real meanF=sumF/rb->bodies->size(); 

	// commulate normal strains from contacts
	// get max force on contacts
	Real maxContactF=0;
	FOREACH(const shared_ptr<Interaction>& I, *rb->interactions){
		if(!I->isReal) continue;
		shared_ptr<BrefcomContact> BC=YADE_PTR_CAST<BrefcomContact>(I->interactionPhysics); assert(BC);
		maxContactF=max(maxContactF,max(BC->Fn,BC->Fs.Length()));
		BrefcomPhysParams* bpp1(YADE_CAST<BrefcomPhysParams*>(Body::byId(I->getId1())->physicalParameters.get()));
		BrefcomPhysParams* bpp2(YADE_CAST<BrefcomPhysParams*>(Body::byId(I->getId2())->physicalParameters.get()));
		bpp1->epsVolumetric+=BC->epsN; bpp1->numContacts+=1;
		bpp2->epsVolumetric+=BC->epsN; bpp2->numContacts+=1;
	}
	unbalancedForce=(useMaxForce?maxF:meanF)/maxContactF;

	FOREACH(const shared_ptr<Interaction>& I, *rb->interactions){
		if(!I->isReal) continue;
		shared_ptr<BrefcomContact> BC=YADE_PTR_CAST<BrefcomContact>(I->interactionPhysics); assert(BC);
		BrefcomPhysParams* bpp1(YADE_CAST<BrefcomPhysParams*>(Body::byId(I->getId1())->physicalParameters.get()));
		BrefcomPhysParams* bpp2(YADE_CAST<BrefcomPhysParams*>(Body::byId(I->getId2())->physicalParameters.get()));
		Real epsVolAvg=.5*((3./bpp1->numContacts)*bpp1->epsVolumetric+(3./bpp2->numContacts)*bpp2->epsVolumetric);
		BC->epsTrans=(epsVolAvg-BC->epsN)/2.;
		//TRVAR5(I->getId1(),I->getId2(),BC->epsTrans,(3./bpp1->numContacts)*bpp1->epsVolumetric,(3./bpp2->numContacts)*bpp2->epsVolumetric);
		//TRVAR4(bpp1->numContacts,bpp1->epsVolumetric,bpp2->numContacts,bpp2->epsVolumetric);
	}
	#if 0
		FOREACH(const shared_ptr<Body>& b, *rb->bodies){
			BrefcomPhysParams* bpp(YADE_PTR_CAST<BrefcomPhysParams>(b->physicalParameters.get()));
			bpp->epsVolumeric*=3/bpp->numContacts;
		}
	#endif


}


/********************** BrefcomMakeContact ****************************/
CREATE_LOGGER(BrefcomMakeContact);


//! @todo Formulas in the following should be verified
void BrefcomMakeContact::go(const shared_ptr<PhysicalParameters>& pp1, const shared_ptr<PhysicalParameters>& pp2, const shared_ptr<Interaction>& interaction){
	const shared_ptr<SpheresContactGeometry>& contGeom=YADE_PTR_CAST<SpheresContactGeometry>(interaction->interactionGeometry);
	assert(contGeom); // for now, don't handle anything other than SpheresContactGeometry

	if(!interaction->isNew && interaction->interactionPhysics){ /* relax */ } 
	else {
		interaction->isNew; // just in case

		const shared_ptr<BodyMacroParameters>& elast1=static_pointer_cast<BodyMacroParameters>(pp1);
		const shared_ptr<BodyMacroParameters>& elast2=static_pointer_cast<BodyMacroParameters>(pp2);

		Real E12=2*elast1->young*elast2->young/(elast1->young+elast2->young); // harmonic Young's modulus average
		//Real nu12=2*elast1->poisson*elast2->poisson/(elast1->poisson+elast2->poisson); // dtto for Poisson ratio 
		Real S12=Mathr::PI*pow(min(contGeom->radius1,contGeom->radius2),2); // "surface" of interaction
		//Real E=(E12 /* was here for Kn:  *S12/d0  */)*((1+alpha)/(beta*(1+nu12)+gamma*(1-alpha*nu12)));
		//Real E=E12; // apply alpha, beta, gamma: garbage values of E !?

		if(!neverDamage) { assert(!isnan(sigmaT)); }

		shared_ptr<BrefcomContact> contPhys(new BrefcomContact());

		contPhys->E=E12;
		contPhys->G=E12*G_over_E;
		contPhys->tanFrictionAngle=tan(.5*(elast1->frictionAngle+elast2->frictionAngle));
		contPhys->undamagedCohesion=sigmaT;
		contPhys->crossSection=S12;
		contPhys->epsCrackOnset=epsCrackOnset;
		contPhys->epsFracture=relDuctility*epsCrackOnset;
		contPhys->omegaThreshold=omegaThreshold;
		// inherited from NormalShearInteracion, used in the timestepper
		contPhys->kn=contPhys->E*contPhys->crossSection;
		contPhys->ks=contPhys->G*contPhys->crossSection;

		if(neverDamage) contPhys->neverDamage=true;
		if(cohesiveThresholdIter<0 || Omega::instance().getCurrentIteration()<cohesiveThresholdIter) contPhys->isCohesive=true;
		else contPhys->isCohesive=false;
		contPhys->dmgTau=dmgTau;
		contPhys->dmgRateExp=dmgRateExp;
		contPhys->plTau=plTau;
		contPhys->plRateExp=plRateExp;

		interaction->interactionPhysics=contPhys;
	}
}




/********************** BrefcomContact ****************************/
CREATE_LOGGER(BrefcomContact);

// !! at least one virtual function in the .cpp file
BrefcomContact::~BrefcomContact(){};


/********************** BrefcomLaw ****************************/
CREATE_LOGGER(BrefcomLaw);

void BrefcomLaw::applyForce(const Vector3r& force, const body_id_t& id1, const body_id_t& id2){
	rootBody->bex.addForce(id1,force);
	rootBody->bex.addForce(id2,-force);
	rootBody->bex.addTorque(id1,(contGeom->contactPoint-contGeom->pos1).Cross(force));
	rootBody->bex.addTorque(id2,(contGeom->contactPoint-contGeom->pos2).Cross(-force));
}

CREATE_LOGGER(ef2_Spheres_Brefcom_BrefcomLaw);

long BrefcomContact::cummBetaIter=0, BrefcomContact::cummBetaCount=0;

Real BrefcomContact::solveBeta(const Real c, const Real N){
	#ifdef YADE_DEBUG
		cummBetaCount++;
	#endif
	const int maxIter=20;
	const Real maxError=1e-12;
	Real f, ret=0.;
	for(int i=0; i<maxIter; i++){
		#ifdef YADE_DEBUG
			cummBetaIter++;
		#endif
		Real aux=c*exp(N*ret)+exp(ret);
		f=log(aux);
		if(fabs(f)<maxError) return ret;
		Real df=(c*N*exp(N*ret)+exp(ret))/aux;
		ret-=f/df;
	}
	LOG_FATAL("No convergence after "<<maxIter<<" iters; c="<<c<<", N="<<N<<", ret="<<ret<<", f="<<f);
	throw runtime_error("ef2_Spheres_Brefcom_BrefcomLaw::solveBeta failed to converge.");
}

Real BrefcomContact::computeDmgOverstress(Real dt){
	if(dmgStrain>=epsN*omega){ // unloading, no viscous stress
		dmgStrain=epsN*omega;
		LOG_TRACE("Elastic/unloading, no viscous overstress");
		return 0.;
	}
	Real c=epsCrackOnset*(1-omega)*pow(dmgTau/dt,dmgRateExp)*pow(epsN*omega-dmgStrain,dmgRateExp-1.);
	Real beta=solveBeta(c,dmgRateExp);
	Real deltaDmgStrain=(epsN*omega-dmgStrain)*exp(beta);
	dmgStrain+=deltaDmgStrain;
	LOG_TRACE("deltaDmgStrain="<<deltaDmgStrain<<", viscous overstress "<<(epsN*omega-dmgStrain)*E);
	/* σN=Kn(εN-εd); dmgOverstress=σN-(1-ω)*Kn*εN=…=Kn(ω*εN-εd) */
	return (epsN*omega-dmgStrain)*E;
}

Real BrefcomContact::computeViscoplScalingFactor(Real sigmaTNorm, Real sigmaTYield,Real dt){
	if(sigmaTNorm<sigmaTYield) return 1.;
	Real c=sigmaTNorm*pow(plTau/(G*dt),plRateExp)*pow(sigmaTNorm-sigmaTYield,plRateExp-1.);
	Real beta=solveBeta(c,plRateExp);
	return 1.-exp(beta)*(1-sigmaTYield/sigmaTNorm);
}

void ef2_Spheres_Brefcom_BrefcomLaw::go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, MetaBody* rootBody){
	//timingDeltas->start();
	SpheresContactGeometry* contGeom=static_cast<SpheresContactGeometry*>(_geom.get());
	BrefcomContact* BC=static_cast<BrefcomContact*>(_phys.get());

	/* kept fully damaged contacts; note that normally the contact is deleted _after_ the BREFCOM_MATERIAL_MODEL,
	 * i.e. if it is 1.0 here, omegaThreshold is >= 1.0 for sure.
	 * &&'ing that just to make sure anyway ...
	 */
	if(BC->omega>=1.0 && BC->omegaThreshold>=1.0) return;

	// shorthands
	Real& epsN(BC->epsN); Vector3r& epsT(BC->epsT); Real& kappaD(BC->kappaD); Real& epsPlSum(BC->epsPlSum); const Real& E(BC->E); const Real& undamagedCohesion(BC->undamagedCohesion); const Real& tanFrictionAngle(BC->tanFrictionAngle); const Real& G(BC->G); const Real& crossSection(BC->crossSection); const Real& omegaThreshold(BC->omegaThreshold); const Real& epsCrackOnset(BC->epsCrackOnset); Real& relResidualStrength(BC->relResidualStrength); const Real& dt=Omega::instance().getTimeStep();  const Real& epsFracture(BC->epsFracture); const bool& neverDamage(BC->neverDamage); const Real& dmgTau(BC->dmgTau); const Real& plTau(BC->plTau);
	/* const Real& transStrainCoeff(BC->transStrainCoeff); const Real& epsTrans(BC->epsTrans); const Real& xiShear(BC->xiShear); */
	Real& omega(BC->omega); Real& sigmaN(BC->sigmaN);  Vector3r& sigmaT(BC->sigmaT); Real& Fn(BC->Fn); Vector3r& Fs(BC->Fs); // for python access

	#define NNAN(a) assert(!isnan(a));
	#define NNANV(v) assert(!isnan(v[0])); assert(!isnan(v[1])); assert(!isnan(v[2]));

	assert(contGeom->hasShear);
	//timingDeltas->checkpoint("setup");
	epsN=contGeom->epsN(); epsT=contGeom->epsT();
	if(isnan(epsN)){
		LOG_ERROR("d0="<<contGeom->d0<<", d1,d2="<<contGeom->d1<<","<<contGeom->d2<<"; pos1,pos2="<<contGeom->pos1<<","<<contGeom->pos2);
	}
	NNAN(epsN); NNANV(epsT);
	// already in SpheresContactGeometry:
	// contGeom->relocateContactPoints(); // allow very large mutual rotations
	if(logStrain && epsN<0){ Real epsN0=epsN; epsN=log(epsN0+1); epsT*=epsN/epsN0; }
	//timingDeltas->checkpoint("geom");
	#ifdef BREFCOM_MATERIAL_MODEL
		BREFCOM_MATERIAL_MODEL
	#else
		sigmaN=E*epsN;
		sigmaT=G*epsT;
	#endif
	//timingDeltas->checkpoint("material");
	if(omega>omegaThreshold){
		I->isReal=false;
		const shared_ptr<Body>& body1=Body::byId(I->getId1(),rootBody), body2=Body::byId(I->getId2(),rootBody); assert(body1); assert(body2);
		const shared_ptr<BrefcomPhysParams>& rbp1=YADE_PTR_CAST<BrefcomPhysParams>(body1->physicalParameters), rbp2=YADE_PTR_CAST<BrefcomPhysParams>(body2->physicalParameters);
		if(BC->isCohesive){rbp1->numBrokenCohesive+=1; rbp2->numBrokenCohesive+=1; rbp1->epsPlBroken+=epsPlSum; rbp2->epsPlBroken+=epsPlSum;}
		LOG_DEBUG("Contact #"<<I->getId1()<<"=#"<<I->getId2()<<" is damaged over thershold ("<<omega<<">"<<omegaThreshold<<") and has been deleted (isReal="<<I->isReal<<")");
		return;
	}
	NNAN(sigmaN); NNANV(sigmaT); NNAN(crossSection);

	Fn=sigmaN*crossSection; BC->normalForce=Fn*contGeom->normal;
	Fs=sigmaT*crossSection; BC->shearForce=Fs;

	applyForceAtContactPoint(BC->normalForce+BC->shearForce, contGeom->contactPoint, I->getId1(), contGeom->pos1, I->getId2(), contGeom->pos2, rootBody);
	//timingDeltas->checkpoint("rest");
}


void BrefcomLaw::action(MetaBody* rootBody){
	if(!functor) functor=shared_ptr<ef2_Spheres_Brefcom_BrefcomLaw>(new ef2_Spheres_Brefcom_BrefcomLaw);
	functor->logStrain=logStrain;
	FOREACH(const shared_ptr<Interaction>& I, *rootBody->interactions){
		if(!I->isReal) continue;
		functor->go(I->interactionGeometry, I->interactionPhysics, I.get(), rootBody);
	}
}


/********************** GLDrawBrefcomContact ****************************/

#include<yade/lib-opengl/OpenGLWrapper.hpp>

CREATE_LOGGER(GLDrawBrefcomContact);

bool GLDrawBrefcomContact::contactLine=true;
bool GLDrawBrefcomContact::dmgLabel=true;
bool GLDrawBrefcomContact::dmgPlane=false;
bool GLDrawBrefcomContact::epsNLabel=true;
bool GLDrawBrefcomContact::epsT=false;
bool GLDrawBrefcomContact::epsTAxes=false;
bool GLDrawBrefcomContact::normal=false;
bool GLDrawBrefcomContact::colorStrain=false;


void GLDrawBrefcomContact::go(const shared_ptr<InteractionPhysics>& ip, const shared_ptr<Interaction>& i, const shared_ptr<Body>& b1, const shared_ptr<Body>& b2, bool wireFrame){
	const shared_ptr<BrefcomContact>& BC=static_pointer_cast<BrefcomContact>(ip);
	const shared_ptr<SpheresContactGeometry>& geom=YADE_PTR_CAST<SpheresContactGeometry>(i->interactionGeometry);

	//Vector3r lineColor(BC->omega,1-BC->omega,0.0); /* damaged links red, undamaged green */
	Vector3r lineColor=Shop::scalarOnColorScale(1.-BC->relResidualStrength);

	if(colorStrain) lineColor=Vector3r(
		min((Real)1.,max((Real)0.,-BC->epsTrans/BC->epsCrackOnset)),
		min((Real)1.,max((Real)0.,BC->epsTrans/BC->epsCrackOnset)),
		min((Real)1.,max((Real)0.,abs(BC->epsTrans)/BC->epsCrackOnset-1)));

	if(contactLine) GLUtils::GLDrawLine(b1->physicalParameters->dispSe3.position,b2->physicalParameters->dispSe3.position,lineColor);
	if(dmgLabel){ GLUtils::GLDrawNum(BC->omega,0.5*(b1->physicalParameters->dispSe3.position+b2->physicalParameters->dispSe3.position),lineColor); }
	else if(epsNLabel){ GLUtils::GLDrawNum(BC->epsN,0.5*(b1->physicalParameters->dispSe3.position+b2->physicalParameters->dispSe3.position),lineColor); }
	if(BC->omega>0 && dmgPlane){
		Real halfSize=sqrt(1-BC->relResidualStrength)*.5*.705*sqrt(BC->crossSection);
		Vector3r midPt=.5*Vector3r(b1->physicalParameters->dispSe3.position+b2->physicalParameters->dispSe3.position);
		glDisable(GL_CULL_FACE);
		glPushMatrix();
			glTranslatev(midPt);
			Quaternionr q; q.Align(Vector3r::UNIT_Z,geom->normal);
			Vector3r axis; Real angle; q.ToAxisAngle(axis,angle);
			glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
			glBegin(GL_POLYGON);
				glColor3v(lineColor); 
				glVertex3d(halfSize,0.,0.);
				glVertex3d(.5*halfSize,.866*halfSize,0.);
				glVertex3d(-.5*halfSize,.866*halfSize,0.);
				glVertex3d(-halfSize,0.,0.);
				glVertex3d(-.5*halfSize,-.866*halfSize,0.);
				glVertex3d(.5*halfSize,-.866*halfSize,0.);
			glEnd();
		glPopMatrix();
	}

	const Vector3r& cp=static_pointer_cast<SpheresContactGeometry>(i->interactionGeometry)->contactPoint;
	if(epsT){
		Real maxShear=(BC->undamagedCohesion-BC->sigmaN*BC->tanFrictionAngle)/BC->G;
		Real relShear=BC->epsT.Length()/maxShear;
		Real scale=.5*geom->d0;
		Vector3r dirShear=BC->epsT; dirShear.Normalize();
		if(epsTAxes){
			GLUtils::GLDrawLine(cp-Vector3r(scale,0,0),cp+Vector3r(scale,0,0));
			GLUtils::GLDrawLine(cp-Vector3r(0,scale,0),cp+Vector3r(0,scale,0));
			GLUtils::GLDrawLine(cp-Vector3r(0,0,scale),cp+Vector3r(0,0,scale));
		}
		GLUtils::GLDrawArrow(cp,cp+dirShear*relShear*scale,Vector3r(1.,0.,0.));
		GLUtils::GLDrawLine(cp+dirShear*relShear*scale,cp+dirShear*scale,Vector3r(.3,.3,.3));

		/* normal strain */ GLUtils::GLDrawArrow(cp,cp+geom->normal*(BC->epsN/maxShear),Vector3r(0.,1.,0.));
	}
	//if(normal) GLUtils::GLDrawArrow(cp,cp+geom->normal*.5*BC->equilibriumDist,Vector3r(0.,1.,0.));
}

struct BodyStats{ short nCohLinks; Real dmgSum; Real epsPlSum; BodyStats(): nCohLinks(0), dmgSum(0), epsPlSum(0.){} };

/********************** BrefcomDamageColorizer ****************************/
void BrefcomDamageColorizer::action(MetaBody* rootBody){
	//vector<pair<short,Real> > bodyDamage; /* number of cohesive interactions per body; cummulative damage of interactions */
	//vector<pair<short,
	vector<BodyStats> bodyStats; bodyStats.resize(rootBody->bodies->size());
	assert(bodyStats[0].nCohLinks==0); // should be initialized by dfault ctor
	FOREACH(const shared_ptr<Interaction>& I, *rootBody->interactions){
		shared_ptr<BrefcomContact> BC=dynamic_pointer_cast<BrefcomContact>(I->interactionPhysics);
		if(!BC || !BC->isCohesive) continue;
		const body_id_t id1=I->getId1(), id2=I->getId2();
		bodyStats[id1].nCohLinks++; bodyStats[id1].dmgSum+=(1-BC->relResidualStrength); bodyStats[id1].epsPlSum+=BC->epsPlSum;
		bodyStats[id2].nCohLinks++; bodyStats[id2].dmgSum+=(1-BC->relResidualStrength); bodyStats[id2].epsPlSum+=BC->epsPlSum;
		//bodyDamage[id1].first++; bodyDamage[id2].first++;
		//bodyDamage[id1].second+=(1-BC->relResidualStrength); bodyDamage[id2].second+=(1-BC->relResidualStrength);
		maxOmega=max(maxOmega,BC->omega);
	}
	FOREACH(shared_ptr<Body> B, *rootBody->bodies){
		body_id_t id=B->getId();
		// add damaged contacts that have already been deleted
		BrefcomPhysParams* bpp=dynamic_cast<BrefcomPhysParams*>(B->physicalParameters.get());
		if(!bpp) continue;
		short cohLinksWhenever=bodyStats[id].nCohLinks+bpp->numBrokenCohesive;
		if(cohLinksWhenever>0){
			bpp->normDmg=(bodyStats[id].dmgSum+bpp->numBrokenCohesive)/cohLinksWhenever;
			bpp->normEpsPl=(bodyStats[id].epsPlSum+bpp->epsPlBroken)/cohLinksWhenever;
		}
		else { bpp->normDmg=0; bpp->normEpsPl=0;}
		B->geometricalModel->diffuseColor=Vector3r(bpp->normDmg,1-bpp->normDmg,B->isDynamic?0:1);
	}
}


