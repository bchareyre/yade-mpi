// 2007,2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include"ConcretePM.hpp"
#include<yade/core/Scene.hpp>
#include<yade/pkg/dem/DemXDofGeom.hpp>
#include<yade/pkg/dem/Shop.hpp>

YADE_PLUGIN((CpmState)(CpmMat)(Ip2_CpmMat_CpmMat_CpmPhys)(Ip2_FrictMat_CpmMat_FrictPhys)(CpmPhys)(Law2_Dem3DofGeom_CpmPhys_Cpm)(Law2_ScGeom_CpmPhys_Cpm)
	#ifdef YADE_OPENGL
		(Gl1_CpmPhys)
	#endif	
		(CpmStateUpdater));


/********************** Ip2_CpmMat_CpmMat_CpmPhys ****************************/


CREATE_LOGGER(Ip2_FrictMat_CpmMat_FrictPhys);
void Ip2_FrictMat_CpmMat_FrictPhys::go(const shared_ptr<Material>& pp1, const shared_ptr<Material>& pp2, const shared_ptr<Interaction>& interaction){
	const shared_ptr<FrictMat>& mat1 = YADE_PTR_CAST<FrictMat>(pp1);
	const shared_ptr<CpmMat>& mat2 = YADE_PTR_CAST<CpmMat>(pp2);
	Ip2_FrictMat_FrictMat_FrictPhys().go(mat1,mat2,interaction);
}




CREATE_LOGGER(Ip2_CpmMat_CpmMat_CpmPhys);
void Ip2_CpmMat_CpmMat_CpmPhys::go(const shared_ptr<Material>& pp1, const shared_ptr<Material>& pp2, const shared_ptr<Interaction>& interaction){
	// no updates of an already existing contact necessary
	if (interaction->phys) return;
	shared_ptr<CpmPhys> cpmPhys(new CpmPhys());
	interaction->phys = cpmPhys;
	CpmMat* mat1 = YADE_CAST<CpmMat*>(pp1.get());
	CpmMat* mat2 = YADE_CAST<CpmMat*>(pp2.get());

	// check unassigned values
	if (!mat1->neverDamage) {
		assert(!isnan(mat1->sigmaT));
		assert(!isnan(mat1->epsCrackOnset));
		assert(!isnan(mat1->crackOpening) || !isnan(mat1->relDuctility));
	}
	if (!mat2->neverDamage) {
		assert(!isnan(mat2->sigmaT));
		assert(!isnan(mat2->epsCrackOnset));
		assert(!isnan(mat2->crackOpening) || !isnan(mat2->relDuctility));
	}

	cpmPhys->damLaw = mat1->damLaw;
	// bodies sharing the same material; no averages necessary
	if (mat1->id>=0 && mat1->id == mat2->id) {
		cpmPhys->E = mat1->young;
		cpmPhys->G = mat1->young*mat1->poisson;
		cpmPhys->tanFrictionAngle = tan(mat1->frictionAngle);
		cpmPhys->undamagedCohesion = mat1->sigmaT;
		cpmPhys->isCohesive = (cohesiveThresholdIter < 0 || scene->iter < cohesiveThresholdIter);
		#define _CPATTR(a) cpmPhys->a=mat1->a
			_CPATTR(epsCrackOnset);
			_CPATTR(crackOpening);
			_CPATTR(relDuctility);
			_CPATTR(neverDamage);
			_CPATTR(dmgTau);
			_CPATTR(dmgRateExp);
			_CPATTR(plTau);
			_CPATTR(plRateExp);
			_CPATTR(isoPrestress);
		#undef _CPATTR
	} else {
		// averaging over both materials
		#define _AVGATTR(a) cpmPhys->a=.5*(mat1->a+mat2->a)
			cpmPhys->E = .5*(mat1->young + mat2->young);
			cpmPhys->G = .5*(mat1->poisson + mat2->poisson)*.5*(mat1->young + mat2->young);
			cpmPhys->tanFrictionAngle = tan(.5*(mat1->frictionAngle + mat2->frictionAngle));
			cpmPhys->undamagedCohesion = .5*(mat1->sigmaT + mat2->sigmaT);
			cpmPhys->isCohesive = (cohesiveThresholdIter < 0 || scene->iter < cohesiveThresholdIter);
			_AVGATTR(epsCrackOnset);
			_AVGATTR(crackOpening);
			_AVGATTR(relDuctility);
			cpmPhys->neverDamage = (mat1->neverDamage || mat2->neverDamage);
			_AVGATTR(dmgTau);
			_AVGATTR(dmgRateExp);
			_AVGATTR(plTau);
			_AVGATTR(plRateExp);
			_AVGATTR(isoPrestress);
		#undef _AVGATTR
	}

	// NOTE: some params are not assigned until in Law2_SomeGeom_CpmPhys_Cpm, since they need geometry as well; those are:
	// 	crossSection, kn, ks, refLength
}




/********************** CpmPhys ****************************/
CREATE_LOGGER(CpmPhys);

// !! at least one virtual function in the .cpp file
CpmPhys::~CpmPhys(){};

long CpmPhys::cummBetaIter=0, CpmPhys::cummBetaCount=0;

Real CpmPhys::solveBeta(const Real c, const Real N){
	#ifdef YADE_DEBUG
		cummBetaCount++;
	#endif
	const int maxIter = 20;
	const Real maxError = 1e-12;
	Real f, ret = 0.;
	for(int i = 0; i < maxIter; i++){
		#ifdef YADE_DEBUG
			cummBetaIter++;
		#endif
		Real aux = c*exp(N*ret)+exp(ret);
		f = log(aux);
		if (fabs(f) < maxError) return ret;
		Real df = (c*N*exp(N*ret)+exp(ret))/aux;
		ret -= f/df;
	}
	LOG_FATAL("No convergence after "<<maxIter<<" iters; c="<<c<<", N="<<N<<", ret="<<ret<<", f="<<f);
	throw runtime_error("CpmPhys::solveBeta failed to converge.");
}

Real CpmPhys::computeDmgOverstress(Real dt){
	if (dmgStrain >= epsN*omega) { // unloading, no viscous stress
		dmgStrain = epsN*omega;
		LOG_TRACE("Elastic/unloading, no viscous overstress");
		return 0.;
	}
	Real c = epsCrackOnset*(1-omega)*pow(dmgTau/dt,dmgRateExp)*pow(epsN*omega-dmgStrain,dmgRateExp-1.);
	Real beta = solveBeta(c,dmgRateExp);
	Real deltaDmgStrain = (epsN*omega-dmgStrain)*exp(beta);
	dmgStrain += deltaDmgStrain;
	LOG_TRACE("deltaDmgStrain="<<deltaDmgStrain<<", viscous overstress "<<(epsN*omega-dmgStrain)*E);
	/* σN=Kn(εN-εd); dmgOverstress=σN-(1-ω)*Kn*εN=…=Kn(ω*εN-εd) */
	return (epsN*omega-dmgStrain)*E;
}

Real CpmPhys::computeViscoplScalingFactor(Real sigmaTNorm, Real sigmaTYield,Real dt){
	if (sigmaTNorm<sigmaTYield) return 1.;
	Real c = undamagedCohesion*pow(plTau/(G*dt),plRateExp)*pow(sigmaTNorm-sigmaTYield,plRateExp-1.);
	Real beta = solveBeta(c,plRateExp);
	//LOG_DEBUG("scaling factor "<<1.-exp(beta)*(1-sigmaTYield/sigmaTNorm));
	return 1.-exp(beta)*(1-sigmaTYield/sigmaTNorm);
}









/********************** Law2_SomeGeom_CpmPhys_Cpm ****************************/
CREATE_LOGGER(Law2_Dem3DofGeom_CpmPhys_Cpm);

#ifdef YADE_CPM_FULL_MODEL_AVAILABLE
	#include"../../../brefcom-mm.hh"
#endif

// #undef CPM_MATERIAL_MODEL (force trunk version of the model)


#ifdef CPM_MATERIAL_MODEL
	#define CPM_MATERIAL_MODEL_A \
		Real& epsNPl(BC->epsNPl);\
		const Real& dt = scene->dt;\
		const Real& dmgTau(BC->dmgTau);\
		const Real& plTau(BC->plTau);\
		const Real& yieldLogSpeed(CPM_LAW2::yieldLogSpeed);\
		const int& yieldSurfType(CPM_LAW2::yieldSurfType);\
		const Real& yieldEllipseShift(CPM_LAW2::yieldEllipseShift);\
		const Real& epsSoft(CPM_LAW2::epsSoft);\
		const Real& relKnSoft(CPM_LAW2::relKnSoft);
#else
	#define CPM_MATERIAL_MODEL_A
#endif


// constitutive law 
#ifdef CPM_MATERIAL_MODEL
	#define CPM_MATERIAL_MODEL_B \
		/* complicated version */ \
		if (epsSoft >= 0) epsN += BC->isoPrestress/E; \
		else { /* take softening into account for the prestress */ \
			Real sigmaSoft=E*epsSoft;\
			if (BC->isoPrestress >= sigmaSoft) epsN += BC -> isoPrestress/E; /* on the non-softened branch yet */ \
			/* otherwise take the regular and softened branches separately (different moduli) */ \
			else epsN += sigmaSoft/E+(BC->isoPrestress-sigmaSoft)/(E*relKnSoft);\
		} \
		CPM_MATERIAL_MODEL
#else
	#define CPM_MATERIAL_MODEL_B \
		/* simplified public model */ \
		epsN += BC->isoPrestress/E; \
		/* very simplified version of the constitutive law */ \
		kappaD = max(max(0.,epsN),kappaD); /* internal variable, max positive strain (non-decreasing) */ \
		/* Real epsFracture = crackOpening/contGeom->refLength; */ \
		omega = isCohesive? funcG(kappaD,epsCrackOnset,epsFracture,neverDamage,damLaw) : 1.; /* damage variable (non-decreasing, as funcG is also non-decreasing) */ \
		sigmaN = (1-(epsN>0?omega:0))*E*epsN; /* damage taken in account in tension only */ \
		sigmaT = G*epsT; /* trial stress */ \
		Real yieldSigmaT = max((Real)0.,undamagedCohesion*(1-omega)-sigmaN*tanFrictionAngle); /* Mohr-Coulomb law with damage */ \
		if (sigmaT.squaredNorm() > yieldSigmaT*yieldSigmaT) { \
			Real scale = yieldSigmaT/sigmaT.norm(); \
			sigmaT *= scale; /* stress return */ \
			epsT *= scale; \
			/* epsPlSum += yieldSigmaT*contGeom->slipToStrainTMax(yieldSigmaT/G);*/ /* adjust strain */ \
		} \
		relResidualStrength = isCohesive? (kappaD<epsCrackOnset? 1. : (1-omega)*(kappaD)/epsCrackOnset) : 0;
#endif


#ifdef YADE_DEBUG
	#define CPM_YADE_DEBUG_A \
		if(isnan(epsN)){\
			LOG_FATAL("refLength="<<contGeom->refLength<<"; pos1="<<contGeom->se31.position<<"; pos2="<<contGeom->se32.position<<"; displacementN="<<contGeom->displacementN());\
			throw runtime_error("!! epsN==NaN !!");\
		}
#else
	#define CPM_YADE_DEBUG_A
#endif


#define YADE_VERIFY(condition) if(!(condition)){LOG_FATAL("Verification `"<<#condition<<"' failed!"); LOG_FATAL("in interaction #"<<I->getId1()<<"+#"<<I->getId2()); Omega::instance().saveSimulation("/tmp/verificationFailed.xml"); throw;}
#define NNAN(a) YADE_VERIFY(!isnan(a));
#define NNANV(v) YADE_VERIFY(!isnan(v[0])); assert(!isnan(v[1])); assert(!isnan(v[2]));



#define CPM_GO_A \
	/* just the first time */ \
	if (I->isFresh(scene)) { \
		Vector3r pos1 = scene->bodies->operator[](I->id1)->state->pos; \
		Vector3r pos2 = scene->bodies->operator[](I->id2)->state->pos; \
		if (scene->isPeriodic) { \
			Vector3r temp = pos2 - pos1; \
			pos1 = scene->cell->wrapShearedPt(pos1); \
			pos2 = pos1 + temp; \
		} \
		Real minRad = (contGeom->refR1 <= 0? contGeom->refR2 : (contGeom->refR2 <=0? contGeom->refR1 : min(contGeom->refR1,contGeom->refR2))); \
		BC->refLength = (pos2 - pos1).norm(); \
		BC->crossSection = Mathr::PI*pow(minRad,2); \
		BC->refPD = contGeom->refR1 + contGeom->refR2 - BC->refLength; \
		BC->kn = BC->crossSection*BC->E/BC->refLength; \
		BC->ks = BC->crossSection*BC->G/BC->refLength; \
		BC->epsFracture = isnan(BC->crackOpening)? BC->epsCrackOnset*BC->relDuctility : BC->crackOpening/(2*minRad); /* *contGeom->refLength */; \
	} \
	\
	/* shorthands */ \
	Real& epsN(BC->epsN); \
	Vector3r& epsT(BC->epsT); \
	Real& kappaD(BC->kappaD); \
	/* Real& epsPlSum(BC->epsPlSum); */ \
	const Real& E(BC->E); \
	const Real& undamagedCohesion(BC->undamagedCohesion); \
	const Real& tanFrictionAngle(BC->tanFrictionAngle); \
	const Real& G(BC->G); \
	const Real& crossSection(BC->crossSection); \
	const Real& omegaThreshold(CPM_LAW2::omegaThreshold); \
	const Real& epsCrackOnset(BC->epsCrackOnset); \
	Real& relResidualStrength(BC->relResidualStrength); \
	/*const Real& crackOpening(BC->crackOpening); */ \
	/*const Real& relDuctility(BC->relDuctility); */ \
	const Real& epsFracture(BC->epsFracture); \
	const int& damLaw(BC->damLaw); \
	const bool& neverDamage(BC->neverDamage); \
	Real& omega(BC->omega); \
	Real& sigmaN(BC->sigmaN); \
	Vector3r& sigmaT(BC->sigmaT); \
	Real& Fn(BC->Fn); \
	Vector3r& Fs(BC->Fs); /* for python access */ \
	const bool& isCohesive(BC->isCohesive); \
	\
	CPM_MATERIAL_MODEL_A

#define CPM_GO_B \
	/* debugging */ \
	CPM_YADE_DEBUG_A \
	\
	NNAN(epsN); NNANV(epsT); \
	\
	/* constitutive law */ \
	CPM_MATERIAL_MODEL_B \
	\
	sigmaN -= BC->isoPrestress; \
	\
	NNAN(kappaD); NNAN(epsFracture); NNAN(omega); \
	NNAN(sigmaN); NNANV(sigmaT); NNAN(crossSection); \
	\
	/* handle broken contacts */ \
	if (epsN>0. && ((isCohesive && omega>omegaThreshold) || !isCohesive)) { \
		/* if (isCohesive) { */ \
			 const shared_ptr<Body>& body1 = Body::byId(I->getId1(),scene), body2 = Body::byId(I->getId2(),scene); assert(body1); assert(body2); \
			 const shared_ptr<CpmState>& st1 = YADE_PTR_CAST<CpmState>(body1->state), st2 = YADE_PTR_CAST<CpmState>(body2->state); \
			/* nice article about openMP::critical vs. scoped locks: http://www.thinkingparallel.com/2006/08/21/scoped-locking-vs-critical-in-openmp-a-personal-shootout/ */ \
			{ boost::mutex::scoped_lock lock(st1->updateMutex); st1->numBrokenCohesive += 1; /* st1->epsPlBroken += epsPlSum; */ } \
			{ boost::mutex::scoped_lock lock(st2->updateMutex); st2->numBrokenCohesive += 1; /* st2->epsPlBroken += epsPlSum; */ } \
		/* } */ \
		scene->interactions->requestErase(I); \
		return; \
	} \
	\
	Fn = sigmaN*crossSection; BC->normalForce = Fn*contGeom->normal; \
	Fs = sigmaT*crossSection; BC->shearForce = Fs;






void Law2_Dem3DofGeom_CpmPhys_Cpm::go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I){
	Dem3DofGeom* contGeom = static_cast<Dem3DofGeom*>(_geom.get());
	CpmPhys* BC = static_cast<CpmPhys*>(_phys.get());
	
	#define CPM_LAW2 Law2_Dem3DofGeom_CpmPhys_Cpm
	CPM_GO_A

	epsN = contGeom->strainN();
	epsT = contGeom->strainT();

	CPM_GO_B

	applyForceAtContactPoint(BC->normalForce + BC->shearForce, contGeom->contactPoint, I->getId1(), contGeom->se31.position, I->getId2(), contGeom->se32.position);
}





CREATE_LOGGER(Law2_ScGeom_CpmPhys_Cpm);
void Law2_ScGeom_CpmPhys_Cpm::go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I){
	ScGeom* contGeom=static_cast<ScGeom*>(_geom.get());
	CpmPhys* BC=static_cast<CpmPhys*>(_phys.get());

	#define CPM_LAW2 Law2_ScGeom_CpmPhys_Cpm
	CPM_GO_A
	
	epsN = - (-BC->refPD + contGeom->penetrationDepth) / BC->refLength;
	epsT = contGeom->rotate(epsT);
	epsT += contGeom->shearIncrement() / (BC->refLength + BC->refPD) ; 

	CPM_GO_B

	Vector3r pos1,pos2;
	if (scene->isPeriodic) {
		pos1 = scene->bodies->operator[](I->id1)->state->pos;
		pos2 = scene->bodies->operator[](I->id2)->state->pos;
		Vector3r temp = pos2 - pos1;
		pos1 = scene->cell->wrapShearedPt(pos1);
		pos2 = pos1 + temp;
	} else {
		pos1 = scene->bodies->operator[](I->id1)->state->pos;
		pos2 = scene->bodies->operator[](I->id2)->state->pos;
	}

	applyForceAtContactPoint(BC->normalForce + BC->shearForce, contGeom->contactPoint, I->getId1(), pos1, I->getId2(), pos2);
}








#ifdef YADE_OPENGL
	/********************** Gl1_CpmPhys ****************************/
	#include<yade/lib/opengl/OpenGLWrapper.hpp>
	#include<yade/lib/opengl/GLUtils.hpp>

	CREATE_LOGGER(Gl1_CpmPhys);

	bool Gl1_CpmPhys::contactLine=true;
	bool Gl1_CpmPhys::dmgLabel=true;
	bool Gl1_CpmPhys::dmgPlane=false;
	bool Gl1_CpmPhys::epsNLabel=true;
	bool Gl1_CpmPhys::epsT=false;
	bool Gl1_CpmPhys::epsTAxes=false;
	bool Gl1_CpmPhys::normal=false;
	Real Gl1_CpmPhys::colorStrainRatio=-1;


	void Gl1_CpmPhys::go(const shared_ptr<IPhys>& ip, const shared_ptr<Interaction>& i, const shared_ptr<Body>& b1, const shared_ptr<Body>& b2, bool wireFrame){
		const shared_ptr<CpmPhys>& BC = static_pointer_cast<CpmPhys>(ip);
		const shared_ptr<GenericSpheresContact>& geom = YADE_PTR_CAST<GenericSpheresContact>(i->geom);
		// FIXME: get the scene for periodicity; ugly!
		Scene* scene=Omega::instance().getScene().get();

		//Vector3r lineColor(BC->omega,1-BC->omega,0.0); /* damaged links red, undamaged green */
		Vector3r lineColor = Shop::scalarOnColorScale(1.-BC->relResidualStrength);

		if(colorStrainRatio>0) lineColor = Shop::scalarOnColorScale(BC->epsN/(BC->epsCrackOnset*colorStrainRatio));

		// FIXME: should be computed by the renderer; for now, use the real values
		//Vector3r pos1=geom->se31.position, pos2=geom->se32.position;
		Vector3r pos1 = scene->bodies->operator[](i->id1)->state->pos, pos2 = scene->bodies->operator[](i->id2)->state->pos;
		/*
		if(scene->isPeriodic){
			pos1=scene->cell->wrapShearedPt(pos1);
			pos2=pos1+(geom->se32.position-geom->se31.position);
		}
		*/
		if (scene->isPeriodic) {
			Vector3r temp = pos2 - pos1;
			pos1 = scene->cell->wrapShearedPt(pos1);
			pos2 = pos1 + temp;
		}

		if (contactLine) GLUtils::GLDrawLine(pos1,pos2,lineColor);
		if (dmgLabel) { GLUtils::GLDrawNum(BC->omega,0.5*(pos1+pos2),lineColor); }
		else if (epsNLabel) { GLUtils::GLDrawNum(BC->epsN,0.5*(pos1+pos2),lineColor); }
		if (BC->omega>0 && dmgPlane) {
			Real halfSize = sqrt(1-BC->relResidualStrength)*.5*.705*sqrt(BC->crossSection);
			Vector3r midPt = .5*Vector3r(pos1+pos2);
			glDisable(GL_CULL_FACE);
			glPushMatrix();
				glTranslatev(midPt);
				Quaternionr q; q.setFromTwoVectors(Vector3r::UnitZ(),geom->normal);
				AngleAxisr aa(q);
				glRotatef(aa.angle()*Mathr::RAD_TO_DEG,aa.axis()[0],aa.axis()[1],aa.axis()[2]);
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

		Vector3r cp = static_pointer_cast<GenericSpheresContact>(i->geom)->contactPoint;
		if (scene->isPeriodic) {cp = scene->cell->wrapShearedPt(cp);}
		if (epsT) {
			Real maxShear = (BC->undamagedCohesion-BC->sigmaN*BC->tanFrictionAngle)/BC->G;
			Real relShear = BC->epsT.norm()/maxShear;
			Real scale = BC->refLength;
			Vector3r dirShear = BC->epsT; dirShear.normalize();
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










/********************** CpmStateUpdater ****************************/
CREATE_LOGGER(CpmStateUpdater);
//Real CpmStateUpdater::maxOmega=0.;
//Real CpmStateUpdater::avgRelResidual=0.;

void CpmStateUpdater::update(Scene* _scene){
	Scene *scene = _scene? _scene : Omega::instance().getScene().get();
	vector<BodyStats> bodyStats; bodyStats.resize(scene->bodies->size());
	assert(bodyStats[0].nCohLinks == 0); // should be initialized by dfault ctor
	avgRelResidual = 0; Real nAvgRelResidual = 0;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if (!I) continue;
		if (!I->isReal()) continue;
		shared_ptr<CpmPhys> phys = dynamic_pointer_cast<CpmPhys>(I->phys);
		if (!phys) continue;
		const Body::id_t id1 = I->getId1(), id2 = I->getId2();
		GenericSpheresContact* geom = YADE_CAST<GenericSpheresContact*>(I->geom.get());

		const Vector3r& n = geom->normal;
		const Real& Fn = phys->Fn;
		const Vector3r& Fs = phys->Fs;
		//stress[i,j] += geom->refLength*(Fn*n[i]*n[j]+0.5*(Fs[i]*n[j]+Fs[j]*n[i]));
		//stress += geom->refLength*(Fn*outer(n,n)+.5*(outer(Fs,n)+outer(n,Fs)));
		Matrix3r stress = phys->refLength*(Fn*n*n.transpose()+.5*(Fs*n.transpose()+n*Fs.transpose()));
		
		bodyStats[id1].stress += stress;
		bodyStats[id2].stress += stress;
		bodyStats[id1].nLinks++; bodyStats[id2].nLinks++;
		
		if (!phys->isCohesive) continue;
		bodyStats[id1].nCohLinks++; bodyStats[id1].dmgSum += (1-phys->relResidualStrength); // bodyStats[id1].epsPlSum += phys->epsPlSum;
		bodyStats[id2].nCohLinks++; bodyStats[id2].dmgSum += (1-phys->relResidualStrength); // bodyStats[id2].epsPlSum += phys->epsPlSum;
		maxOmega = max(maxOmega,phys->omega);
		avgRelResidual += phys->relResidualStrength;
		nAvgRelResidual += 1;
		for (int i=0; i<3; i++) {
			for (int j=0; j<3; j++) {
				bodyStats[id1].dmgRhs += n*n.transpose()*(1-phys->relResidualStrength);
				bodyStats[id2].dmgRhs += n*n.transpose()*(1-phys->relResidualStrength);
			}
		}
	}
	Matrix3r m3i; m3i<<0.4,-0.1,-0.1, -0.1,0.4,-0.1, -0.1,-0.1,0.4; // inversion of Matrix3r(3,1,1, 1,3,1, 1,1,3)
	Vector3r temp;
	FOREACH(shared_ptr<Body> B, *scene->bodies){
		if (!B) continue;
		const Body::id_t& id = B->getId();
		// add damaged contacts that have already been deleted
		CpmState* state = dynamic_cast<CpmState*>(B->state.get());
		if (!state) continue;
		state->stress = bodyStats[id].stress;
		int cohLinksWhenever = bodyStats[id].nCohLinks+state->numBrokenCohesive;
		if (cohLinksWhenever>0) {
			state->normDmg = (bodyStats[id].dmgSum+state->numBrokenCohesive)/cohLinksWhenever;
			// state->normEpsPl = (bodyStats[id].epsPlSum+state->epsPlBroken)/cohLinksWhenever;
			if (state->normDmg>1) {
				LOG_WARN("#"<<id<<" normDmg="<<state->normDmg<<" nCohLinks="<<bodyStats[id].nCohLinks<<", numBrokenCohesive="<<state->numBrokenCohesive<<", dmgSum="<<bodyStats[id].dmgSum<<", numAllCohLinks"<<cohLinksWhenever);
			}
			Matrix3r& dmgRhs = bodyStats[id].dmgRhs;
			dmgRhs *= 15./cohLinksWhenever;
			temp = m3i*dmgRhs.diagonal();
			Matrix3r& damageTensor = state->damageTensor;
			for (int i=0; i<3; i++) { damageTensor(i,i) = temp(i); }
			damageTensor(0,1) = damageTensor(1,0) = dmgRhs(0,1);
			damageTensor(1,2) = damageTensor(2,1) = dmgRhs(1,2);
			damageTensor(2,0) = damageTensor(0,2) = dmgRhs(2,0);
		}
		else { state->normDmg = 0; /*state->normEpsPl=0;*/ state->damageTensor = Matrix3r::Zero(); }
		B->shape->color = Vector3r(state->normDmg,1-state->normDmg,B->state->blockedDOFs==State::DOF_ALL?0:1);
		nAvgRelResidual += 0.5*state->numBrokenCohesive; // add half or broken interactions, other body has the other half
		Sphere* sphere = dynamic_cast<Sphere*>(B->shape.get());
		if(!sphere) continue;
		Real& r = sphere->radius;
		state->stress = bodyStats[id].stress/(4/3.*Mathr::PI*r*r*r/.62)*.5;
	}
	avgRelResidual /= nAvgRelResidual;
}





#undef YADE_VERIFY
#undef NNAN
#undef NNANV
