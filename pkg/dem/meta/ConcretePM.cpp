// 2007,2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include"ConcretePM.hpp"
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-dem/DemXDofGeom.hpp>
#include<yade/pkg-dem/Shop.hpp>

YADE_PLUGIN((CpmMat)(Ip2_CpmMat_CpmMat_CpmPhys)(CpmPhys)(Law2_Dem3DofGeom_CpmPhys_Cpm)(CpmGlobalCharacteristics)
	#ifdef YADE_OPENGL
		(GLDrawCpmPhys)
	#endif	
		(CpmStateUpdater));


/********************** Ip2_CpmMat_CpmMat_CpmPhys ****************************/

CREATE_LOGGER(Ip2_CpmMat_CpmMat_CpmPhys);

void Ip2_CpmMat_CpmMat_CpmPhys::go(const shared_ptr<PhysicalParameters>& pp1, const shared_ptr<PhysicalParameters>& pp2, const shared_ptr<Interaction>& interaction){
	if(interaction->interactionPhysics) return; 

	Dem3DofGeom* contGeom=YADE_CAST<Dem3DofGeom*>(interaction->interactionGeometry.get());
	assert(contGeom);

	const shared_ptr<BodyMacroParameters>& elast1=static_pointer_cast<BodyMacroParameters>(pp1);
	const shared_ptr<BodyMacroParameters>& elast2=static_pointer_cast<BodyMacroParameters>(pp2);

	Real E12=2*elast1->young*elast2->young/(elast1->young+elast2->young); // harmonic Young's modulus average
	//Real nu12=2*elast1->poisson*elast2->poisson/(elast1->poisson+elast2->poisson); // dtto for Poisson ratio 
	Real minRad=(contGeom->refR1<=0?contGeom->refR2:(contGeom->refR2<=0?contGeom->refR1:min(contGeom->refR1,contGeom->refR2)));
	Real S12=Mathr::PI*pow(minRad,2); // "surface" of interaction
	//Real E=(E12 /* was here for Kn:  *S12/d0  */)*((1+alpha)/(beta*(1+nu12)+gamma*(1-alpha*nu12)));
	//Real E=E12; // apply alpha, beta, gamma: garbage values of E !?

	if(!neverDamage) { assert(!isnan(sigmaT)); }

	shared_ptr<CpmPhys> contPhys(new CpmPhys());

	contPhys->E=E12;
	contPhys->G=E12*G_over_E;
	contPhys->tanFrictionAngle=tan(.5*(elast1->frictionAngle+elast2->frictionAngle));
	contPhys->undamagedCohesion=sigmaT;
	contPhys->crossSection=S12;
	contPhys->epsCrackOnset=epsCrackOnset;
	contPhys->epsFracture=relDuctility*epsCrackOnset;
	// inherited from NormalShearInteracion, used in the timestepper
	contPhys->kn=contPhys->E*contPhys->crossSection;
	contPhys->ks=contPhys->G*contPhys->crossSection;

	if(neverDamage) contPhys->neverDamage=true;
	if(cohesiveThresholdIter<0 || (Omega::instance().getCurrentIteration()<cohesiveThresholdIter)) contPhys->isCohesive=true;
	else contPhys->isCohesive=false;
	contPhys->dmgTau=dmgTau;
	contPhys->dmgRateExp=dmgRateExp;
	contPhys->plTau=plTau;
	contPhys->plRateExp=plRateExp;
	contPhys->isoPrestress=isoPrestress;

	interaction->interactionPhysics=contPhys;
}




/********************** CpmPhys ****************************/
CREATE_LOGGER(CpmPhys);

// !! at least one virtual function in the .cpp file
CpmPhys::~CpmPhys(){};

CREATE_LOGGER(Law2_Dem3DofGeom_CpmPhys_Cpm);

long CpmPhys::cummBetaIter=0, CpmPhys::cummBetaCount=0;

Real CpmPhys::solveBeta(const Real c, const Real N){
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
	throw runtime_error("Law2_Dem3DofGeom_CpmPhys_Cpm::solveBeta failed to converge.");
}

Real CpmPhys::computeDmgOverstress(Real dt){
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

Real CpmPhys::computeViscoplScalingFactor(Real sigmaTNorm, Real sigmaTYield,Real dt){
	if(sigmaTNorm<sigmaTYield) return 1.;
	Real c=undamagedCohesion*pow(plTau/(G*dt),plRateExp)*pow(sigmaTNorm-sigmaTYield,plRateExp-1.);
	Real beta=solveBeta(c,plRateExp);
	//LOG_DEBUG("scaling factor "<<1.-exp(beta)*(1-sigmaTYield/sigmaTNorm));
	return 1.-exp(beta)*(1-sigmaTYield/sigmaTNorm);
}


/********************** Law2_Dem3DofGeom_CpmPhys_Cpm ****************************/

/// yield surface parameters
int  Law2_Dem3DofGeom_CpmPhys_Cpm::yieldSurfType=2;
Real Law2_Dem3DofGeom_CpmPhys_Cpm::yieldLogSpeed=.1;
Real Law2_Dem3DofGeom_CpmPhys_Cpm::yieldEllipseShift=0.; // deprecated
/// compressive plasticity parameters
// approximates confinement -20MPa precisely, -100MPa a little over, -200 and -400 are OK (secant)
Real Law2_Dem3DofGeom_CpmPhys_Cpm::epsSoft=-3e-3; // deactivated if >=0
Real Law2_Dem3DofGeom_CpmPhys_Cpm::relKnSoft=.25;
// >=1. to deactivate (never delete damaged contacts)
Real Law2_Dem3DofGeom_CpmPhys_Cpm::omegaThreshold=1.;

#ifdef YADE_CPM_FULL_MODEL_AVAILABLE
	#include"../../../../brefcom-mm.hh"
#endif


void Law2_Dem3DofGeom_CpmPhys_Cpm::go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, MetaBody* rootBody){
	Dem3DofGeom* contGeom=static_cast<Dem3DofGeom*>(_geom.get());
	CpmPhys* BC=static_cast<CpmPhys*>(_phys.get());

	// shorthands
		Real& epsN(BC->epsN);
		Vector3r& epsT(BC->epsT); Real& kappaD(BC->kappaD); Real& epsPlSum(BC->epsPlSum); const Real& E(BC->E); const Real& undamagedCohesion(BC->undamagedCohesion); const Real& tanFrictionAngle(BC->tanFrictionAngle); const Real& G(BC->G); const Real& crossSection(BC->crossSection); const Real& omegaThreshold(Law2_Dem3DofGeom_CpmPhys_Cpm::omegaThreshold); const Real& epsCrackOnset(BC->epsCrackOnset); Real& relResidualStrength(BC->relResidualStrength); const Real& epsFracture(BC->epsFracture); const bool& neverDamage(BC->neverDamage); Real& omega(BC->omega); Real& sigmaN(BC->sigmaN);  Vector3r& sigmaT(BC->sigmaT); Real& Fn(BC->Fn); Vector3r& Fs(BC->Fs); // for python access
	 const bool& isCohesive(BC->isCohesive);

	#ifdef CPM_MATERIAL_MODEL
		Real& epsNPl(BC->epsNPl); const Real& dt=Omega::instance().getTimeStep(); const Real& dmgTau(BC->dmgTau); const Real& plTau(BC->plTau);const Real& yieldLogSpeed(Law2_Dem3DofGeom_CpmPhys_Cpm::yieldLogSpeed); const int& yieldSurfType(Law2_Dem3DofGeom_CpmPhys_Cpm::yieldSurfType); const Real& yieldEllipseShift(Law2_Dem3DofGeom_CpmPhys_Cpm::yieldEllipseShift); const Real& epsSoft(Law2_Dem3DofGeom_CpmPhys_Cpm::epsSoft); const Real& relKnSoft(Law2_Dem3DofGeom_CpmPhys_Cpm::relKnSoft); 
	#endif


	epsN=contGeom->strainN(); epsT=contGeom->strainT();
	
	// debugging
		#define YADE_VERIFY(condition) if(!(condition)){LOG_FATAL("Verification `"<<#condition<<"' failed!"); LOG_FATAL("in interaction #"<<I->getId1()<<"+#"<<I->getId2()); Omega::instance().saveSimulation("/tmp/verificationFailed.xml"); throw;}

		#define NNAN(a) YADE_VERIFY(!isnan(a));
		#define NNANV(v) YADE_VERIFY(!isnan(v[0])); assert(!isnan(v[1])); assert(!isnan(v[2]));
		#ifdef YADE_DEBUG
			if(isnan(epsN)){
				LOG_FATAL("refLength="<<contGeom->refLength<<"; pos1="<<contGeom->se31.position<<"; pos2="<<contGeom->se32.position<<"; displacementN="<<contGeom->displacementN());
			throw runtime_error("!! epsN==NaN !!");
			}
			NNAN(epsN); NNANV(epsT);
		#endif
		NNAN(epsN); NNANV(epsT);

	// constitutive law 
	#ifdef CPM_MATERIAL_MODEL
		// complicated version
		if(epsSoft>=0)	epsN+=BC->isoPrestress/E;
		else{ // take softening into account for the prestress
			Real sigmaSoft=E*epsSoft;
			if(BC->isoPrestress>=sigmaSoft) epsN+=BC->isoPrestress/E; // on the non-softened branch yet
			// otherwise take the regular and softened branches separately (different moduli)
			else epsN+=sigmaSoft/E+(BC->isoPrestress-sigmaSoft)/(E*relKnSoft);
		}
		CPM_MATERIAL_MODEL
	#else
		// simplified public model
		epsN+=BC->isoPrestress/E;
		// very simplified version of the constitutive law
		kappaD=max(max(0.,epsN),kappaD); // internal variable, max positive strain (non-decreasing)
		omega=isCohesive?funcG(kappaD,epsCrackOnset,epsFracture,neverDamage):1.; // damage variable (non-decreasing, as funcG is also non-decreasing)
		sigmaN=(1-(epsN>0?omega:0))*E*epsN; // damage taken in account in tension only
		sigmaT=G*epsT; // trial stress
		Real yieldSigmaT=max((Real)0.,undamagedCohesion*(1-omega)-sigmaN*tanFrictionAngle); // Mohr-Coulomb law with damage
		if(sigmaT.SquaredLength()>yieldSigmaT*yieldSigmaT){
			sigmaT*=yieldSigmaT/sigmaT.Length(); // stress return
			epsPlSum+=yieldSigmaT*contGeom->slipToStrainTMax(yieldSigmaT/G); // adjust strain
		}
		relResidualStrength=isCohesive?(kappaD<epsCrackOnset?1.:(1-omega)*(kappaD)/epsCrackOnset):0;
	#endif
	sigmaN-=BC->isoPrestress;

	NNAN(kappaD); NNAN(epsCrackOnset); NNAN(epsFracture); NNAN(omega);
	NNAN(sigmaN); NNANV(sigmaT); NNAN(crossSection);

	// handle broken contacts
	if(epsN>0. && ((isCohesive && omega>omegaThreshold) || !isCohesive)){
		if(isCohesive){
			const shared_ptr<Body>& body1=Body::byId(I->getId1(),rootBody), body2=Body::byId(I->getId2(),rootBody); assert(body1); assert(body2);
			const shared_ptr<CpmMat>& rbp1=YADE_PTR_CAST<CpmMat>(body1->physicalParameters), rbp2=YADE_PTR_CAST<CpmMat>(body2->physicalParameters);
			// nice article about openMP::critical vs. scoped locks: http://www.thinkingparallel.com/2006/08/21/scoped-locking-vs-critical-in-openmp-a-personal-shootout/
			{ boost::mutex::scoped_lock lock(rbp1->updateMutex); rbp1->numBrokenCohesive+=1; rbp1->epsPlBroken+=epsPlSum; }
			{ boost::mutex::scoped_lock lock(rbp2->updateMutex); rbp2->numBrokenCohesive+=1; rbp2->epsPlBroken+=epsPlSum; }
		}
		rootBody->interactions->requestErase(I->getId1(),I->getId2());
		return;
	}

	Fn=sigmaN*crossSection; BC->normalForce=Fn*contGeom->normal;
	Fs=sigmaT*crossSection; BC->shearForce=Fs;

	applyForceAtContactPoint(BC->normalForce+BC->shearForce, contGeom->contactPoint, I->getId1(), contGeom->se31.position, I->getId2(), contGeom->se32.position, rootBody);
}

#ifdef YADE_OPENGL
	/********************** GLDrawCpmPhys ****************************/
	#include<yade/lib-opengl/OpenGLWrapper.hpp>
	#include<yade/lib-opengl/GLUtils.hpp>

	CREATE_LOGGER(GLDrawCpmPhys);

	bool GLDrawCpmPhys::contactLine=true;
	bool GLDrawCpmPhys::dmgLabel=true;
	bool GLDrawCpmPhys::dmgPlane=false;
	bool GLDrawCpmPhys::epsNLabel=true;
	bool GLDrawCpmPhys::epsT=false;
	bool GLDrawCpmPhys::epsTAxes=false;
	bool GLDrawCpmPhys::normal=false;
	bool GLDrawCpmPhys::colorStrain=false;


	void GLDrawCpmPhys::go(const shared_ptr<InteractionPhysics>& ip, const shared_ptr<Interaction>& i, const shared_ptr<Body>& b1, const shared_ptr<Body>& b2, bool wireFrame){
		const shared_ptr<CpmPhys>& BC=static_pointer_cast<CpmPhys>(ip);
		const shared_ptr<Dem3DofGeom>& geom=YADE_PTR_CAST<Dem3DofGeom>(i->interactionGeometry);

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

		const Vector3r& cp=static_pointer_cast<Dem3DofGeom>(i->interactionGeometry)->contactPoint;
		if(epsT){
			Real maxShear=(BC->undamagedCohesion-BC->sigmaN*BC->tanFrictionAngle)/BC->G;
			Real relShear=BC->epsT.Length()/maxShear;
			Real scale=.5*geom->refLength;
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
#endif

/********************** CpmGlobalCharacteristics ****************************/
/*** DEPRECATED ***/
#if 0
CREATE_LOGGER(CpmGlobalCharacteristics);
void CpmGlobalCharacteristics::compute(MetaBody* rb, bool useMaxForce){
	rb->bex.sync();

	// 1. reset volumetric strain (cummulative in the next loop)
	// 2. get maximum force on a body and sum of all forces (for averaging)
	Real sumF=0,maxF=0,currF;
	FOREACH(const shared_ptr<Body>& b, *rb->bodies){
	CpmMat* bpp(YADE_CAST<CpmMat*>(b->physicalParameters.get()));
		bpp->epsVolumetric=0;
		bpp->numContacts=0;
		currF=rb->bex.getForce(b->id).Length(); maxF=max(currF,maxF); sumF+=currF;
	}
	Real meanF=sumF/rb->bodies->size(); 

	// commulate normal strains from contacts
	// get max force on contacts
	Real maxContactF=0;
	FOREACH(const shared_ptr<Interaction>& I, *rb->interactions){
		if(!I->isReal()) continue;
		shared_ptr<CpmPhys> BC=YADE_PTR_CAST<CpmPhys>(I->interactionPhysics); assert(BC);
		maxContactF=max(maxContactF,max(BC->Fn,BC->Fs.Length()));
		CpmMat* bpp1(YADE_CAST<CpmMat*>(Body::byId(I->getId1())->physicalParameters.get()));
		CpmMat* bpp2(YADE_CAST<CpmMat*>(Body::byId(I->getId2())->physicalParameters.get()));
		bpp1->epsVolumetric+=BC->epsN; bpp1->numContacts+=1;
		bpp2->epsVolumetric+=BC->epsN; bpp2->numContacts+=1;
	}
	unbalancedForce=(useMaxForce?maxF:meanF)/maxContactF;

	FOREACH(const shared_ptr<Interaction>& I, *rb->interactions){
		if(!I->isReal()) continue;
		shared_ptr<CpmPhys> BC=YADE_PTR_CAST<CpmPhys>(I->interactionPhysics); assert(BC);
		CpmMat* bpp1(YADE_CAST<CpmMat*>(Body::byId(I->getId1())->physicalParameters.get()));
		CpmMat* bpp2(YADE_CAST<CpmMat*>(Body::byId(I->getId2())->physicalParameters.get()));
		Real epsVolAvg=.5*((3./bpp1->numContacts)*bpp1->epsVolumetric+(3./bpp2->numContacts)*bpp2->epsVolumetric);
		BC->epsTrans=(epsVolAvg-BC->epsN)/2.;
		//TRVAR5(I->getId1(),I->getId2(),BC->epsTrans,(3./bpp1->numContacts)*bpp1->epsVolumetric,(3./bpp2->numContacts)*bpp2->epsVolumetric);
		//TRVAR4(bpp1->numContacts,bpp1->epsVolumetric,bpp2->numContacts,bpp2->epsVolumetric);
	}
	#if 0
		FOREACH(const shared_ptr<Body>& b, *rb->bodies){
			CpmMat* bpp(YADE_PTR_CAST<CpmMat>(b->physicalParameters.get()));
			bpp->epsVolumeric*=3/bpp->numContacts;
		}
	#endif
}
#endif

/********************** CpmStateUpdater ****************************/
CREATE_LOGGER(CpmStateUpdater);
Real CpmStateUpdater::maxOmega=0.;

void CpmStateUpdater::update(MetaBody* _rootBody){
	MetaBody *rootBody=_rootBody?_rootBody:Omega::instance().getRootBody().get();
	vector<BodyStats> bodyStats; bodyStats.resize(rootBody->bodies->size());
	assert(bodyStats[0].nCohLinks==0); // should be initialized by dfault ctor
	FOREACH(const shared_ptr<Interaction>& I, *rootBody->interactions){
		if(!I->isReal()) continue;
		shared_ptr<CpmPhys> phys=dynamic_pointer_cast<CpmPhys>(I->interactionPhysics);
		if(!phys) continue;
		const body_id_t id1=I->getId1(), id2=I->getId2();
		Dem3DofGeom* geom=YADE_CAST<Dem3DofGeom*>(I->interactionGeometry.get());
		
		Vector3r stress=(1./phys->crossSection)*(phys->normalForce+phys->shearForce);
		const Vector3r& p1(geom->se31.position); const Vector3r& cp(geom->contactPoint);
		// force towards the body is negative, away from it is positive (compression/tension)
		for(int i=0; i<3; i++){
			stress[i]*=cp[i]>p1[i] ? 1. : -1.;
		}
		bodyStats[id1].sigma+=stress; bodyStats[id2].sigma+=stress;
		bodyStats[id1].tau+=stress.Cross(cp-geom->se31.position);
		bodyStats[id2].tau+=stress.Cross(cp-geom->se32.position);
		bodyStats[id1].nLinks++; bodyStats[id2].nLinks++;
		
		if(!phys->isCohesive) continue;
		bodyStats[id1].nCohLinks++; bodyStats[id1].dmgSum+=(1-phys->relResidualStrength); bodyStats[id1].epsPlSum+=phys->epsPlSum;
		bodyStats[id2].nCohLinks++; bodyStats[id2].dmgSum+=(1-phys->relResidualStrength); bodyStats[id2].epsPlSum+=phys->epsPlSum;
		maxOmega=max(maxOmega,phys->omega);
	}
	FOREACH(shared_ptr<Body> B, *rootBody->bodies){
		const body_id_t& id=B->getId();
		// add damaged contacts that have already been deleted
		CpmMat* bpp=dynamic_cast<CpmMat*>(B->physicalParameters.get());
		if(!bpp) continue;
		bpp->sigma=bodyStats[id].sigma;
		bpp->tau=bodyStats[id].tau;
		int cohLinksWhenever=bodyStats[id].nCohLinks+bpp->numBrokenCohesive;
		if(cohLinksWhenever>0){
			bpp->normDmg=(bodyStats[id].dmgSum+bpp->numBrokenCohesive)/cohLinksWhenever;
			bpp->normEpsPl=(bodyStats[id].epsPlSum+bpp->epsPlBroken)/cohLinksWhenever;
			if(bpp->normDmg>1){
				LOG_WARN("#"<<id<<" normDmg="<<bpp->normDmg<<" nCohLinks="<<bodyStats[id].nCohLinks<<", numBrokenCohesive="<<bpp->numBrokenCohesive<<", dmgSum="<<bodyStats[id].dmgSum<<", numAllCohLinks"<<cohLinksWhenever);
			}
		}
		else { bpp->normDmg=0; bpp->normEpsPl=0;}
		B->interactingGeometry->diffuseColor=Vector3r(bpp->normDmg,1-bpp->normDmg,B->isDynamic?0:1);
	}
}

