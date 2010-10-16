// 2010 © Chiara Modenese <c.modenese@gmail.com> 


#include"HertzMindlin.hpp"
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>

#define pi 3.14159265 

YADE_PLUGIN(
	(MindlinPhys)
	(Ip2_FrictMat_FrictMat_MindlinPhys)
	(Law2_ScGeom_MindlinPhys_MindlinDeresiewitz)
	(Law2_ScGeom_MindlinPhys_HertzWithLinearShear)
	(Law2_ScGeom_MindlinPhys_Mindlin)
);

Real Law2_ScGeom_MindlinPhys_Mindlin::Real0=0;
Real Law2_ScGeom_MindlinPhys_Mindlin::getfrictionDissipation() {return (Real) frictionDissipation;}
Real Law2_ScGeom_MindlinPhys_Mindlin::getshearEnergy() {return (Real) shearEnergy;}
Real Law2_ScGeom_MindlinPhys_Mindlin::getnormDampDissip() {return (Real) normDampDissip;}
Real Law2_ScGeom_MindlinPhys_Mindlin::getshearDampDissip() {return (Real) shearDampDissip;}

/******************** MindlinPhys *****************************/
CREATE_LOGGER(MindlinPhys);

MindlinPhys::~MindlinPhys(){}; // destructor


/******************** Ip2_FrictMat_FrictMat_MindlinPhys *******/
CREATE_LOGGER(Ip2_FrictMat_FrictMat_MindlinPhys);

void Ip2_FrictMat_FrictMat_MindlinPhys::go(const shared_ptr<Material>& b1,const shared_ptr<Material>& b2, const shared_ptr<Interaction>& interaction){
	if(interaction->phys) return; // no updates of an already existing contact necessary
	shared_ptr<MindlinPhys> mindlinPhys(new MindlinPhys());
	interaction->phys = mindlinPhys;
	FrictMat* mat1 = YADE_CAST<FrictMat*>(b1.get());
	FrictMat* mat2 = YADE_CAST<FrictMat*>(b2.get());

	
	/* from interaction physics */
	Real Ea = mat1->young;
	Real Eb = mat2->young;
	Real Va = mat1->poisson;
	Real Vb = mat2->poisson;
	Real fa = mat1->frictionAngle;
	Real fb = mat2->frictionAngle;


	/* from interaction geometry */
	ScGeom* scg = YADE_CAST<ScGeom*>(interaction->geom.get());		
	Real Da = scg->radius1; 
	Real Db = scg->radius2; 
	Vector3r normal=scg->normal; 


	/* calculate stiffness coefficients */
	Real Ga = Ea/(2*(1+Va));
	Real Gb = Eb/(2*(1+Vb));
	Real G = (Ga+Gb)/2; // average of shear modulus
	Real V = (Va+Vb)/2; // average of poisson's ratio
	Real E = Ea*Eb/((1.-std::pow(Va,2))*Eb+(1.-std::pow(Vb,2))*Ea); // Young modulus
	Real R = Da*Db/(Da+Db); // equivalent radius
	Real Kno = 4./3.*E*sqrt(R); // coefficient for normal stiffness
	Real Kso = 2*sqrt(4*R)*G/(2-V); // coefficient for shear stiffness
	Real frictionAngle = std::min(fa,fb);

	Real Adhesion = 4.*pi*R*gamma; // calculate adhesion force as predicted by DMT theory

	/* pass values calculated from above to MindlinPhys */
	mindlinPhys->tangensOfFrictionAngle = std::tan(frictionAngle); 
	mindlinPhys->prevNormal = scg->normal;
	mindlinPhys->kno = Kno; // this is just a coeff
	mindlinPhys->kso = Kso; // this is just a coeff
	mindlinPhys->adhesionForce = Adhesion;

	/* compute viscous coefficients */
	if(en && betan) throw std::invalid_argument("Ip2_FrictMat_FrictMat_MindlinPhys: only one of en, betan can be specified.");
	if(es && betas) throw std::invalid_argument("Ip2_FrictMat_FrictMat_MindlinPhys: only one of es, betas can be specified.");

	// en specified, compute betan
	if(en){
		Real logE=log((*en)(mat1->id,mat2->id));
		mindlinPhys->betan=-logE/sqrt(pow(Mathr::PI,2)+pow(logE,2));
	}
	// betan specified, use that value directly; otherwise give zero
	else mindlinPhys->betan=betan ? (*betan)(mat1->id,mat2->id) : 0;

	// same for es, except that when not given, use the value of betan
	if(es){
		Real logE=log((*es)(mat1->id,mat2->id));
		mindlinPhys->betas=-logE/sqrt(pow(Mathr::PI,2)+pow(logE,2));
	}
	else mindlinPhys->betas=betas ? (*betas)(mat1->id,mat2->id) : mindlinPhys->betan;
}


/* Function to count the number of adhesive contacts in the simulation at each time step */
Real Law2_ScGeom_MindlinPhys_Mindlin::contactsAdhesive() // It is returning something rather than zero only if includeAdhesion is set to true
{
	Real contactsAdhesive=0;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		MindlinPhys* phys = dynamic_cast<MindlinPhys*>(I->phys.get());
		if (phys->isAdhesive) {contactsAdhesive += 1;}
	}
	return contactsAdhesive;
}

/* Function to get the NORMAL elastic potential energy of the system */
Real Law2_ScGeom_MindlinPhys_Mindlin::normElastEnergy()
{
	Real normEnergy=0;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		ScGeom* scg = dynamic_cast<ScGeom*>(I->geom.get());
		MindlinPhys* phys = dynamic_cast<MindlinPhys*>(I->phys.get());
		if (phys) {
			if (includeAdhesion) {normEnergy += (std::pow(scg->penetrationDepth,5./2.)*2./5.*phys->kno - phys->adhesionForce*scg->penetrationDepth);}
			else {normEnergy += std::pow(scg->penetrationDepth,5./2.)*2./5.*phys->kno;} // work done in the normal direction. NOTE: this is the integral
			}
	}
	return normEnergy;
}

void Law2_ScGeom_MindlinPhys_MindlinDeresiewitz::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact){
	Body::id_t id1(contact->getId1()), id2(contact->getId2());
	ScGeom* geom = static_cast<ScGeom*>(ig.get());
	MindlinPhys* phys=static_cast<MindlinPhys*>(ip.get());	
	const Real uN=geom->penetrationDepth;
	if (uN<0) {scene->interactions->requestErase(id1,id2); return;}
	// normal force
	Real Fn=phys->kno*pow(uN,3/2.);
	phys->normalForce=Fn*geom->normal;
	// exactly zero would not work with the shear formulation, and would give zero shear force anyway
	if(Fn==0) return;
	//phys->kn=3./2.*phys->kno*std::pow(uN,0.5); // update stiffness, not needed

	// contact radius
	Real R=geom->radius1*geom->radius2/(geom->radius1+geom->radius2);
	phys->radius=pow(Fn*pow(R,3/2.)/phys->kno,1/3.);
	
	// shear force: transform, but keep the old value for now
	geom->rotate(phys->usTotal);
	Vector3r usOld=phys->usTotal;
	Vector3r dUs=geom->shearIncrement();
	phys->usTotal-=dUs;

#if 0
	Vector3r shearIncrement;
	shearIncrement=geom->shearIncrement();
	Fs-=ks*shearIncrement;
	// Mohr-Coulomb slip
	Real maxFs2=pow(Fn,2)*pow(phys->tangensOfFrictionAngle,2);
	if(Fs.squaredNorm()>maxFs2) Fs*=sqrt(maxFs2)/Fs.norm();
#endif
	// apply forces
	Vector3r f=-phys->normalForce-phys->shearForce; 
	scene->forces.addForce(id1,f);
	scene->forces.addForce(id2,-f);
	scene->forces.addTorque(id1,(geom->radius1-.5*geom->penetrationDepth)*geom->normal.cross(f));
	scene->forces.addTorque(id2,(geom->radius2-.5*geom->penetrationDepth)*geom->normal.cross(f));
}

void Law2_ScGeom_MindlinPhys_HertzWithLinearShear::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact){
	Body::id_t id1(contact->getId1()), id2(contact->getId2());
	ScGeom* geom = static_cast<ScGeom*>(ig.get());
	MindlinPhys* phys=static_cast<MindlinPhys*>(ip.get());	
	const Real uN=geom->penetrationDepth;
	if (uN<0) {scene->interactions->requestErase(id1,id2); return;}
	// normal force
	Real Fn=phys->kno*pow(uN,3/2.);
	phys->normalForce=Fn*geom->normal;
	//phys->kn=3./2.*phys->kno*std::pow(uN,0.5); // update stiffness, not needed
	
	// shear force
	Vector3r& Fs=geom->rotate(phys->shearForce);
	Real ks= nonLin>0 ? phys->kso*std::pow(uN,0.5) : phys->kso;
	Vector3r shearIncrement;
	if(nonLin>1){
		State *de1=Body::byId(id1,scene)->state.get(), *de2=Body::byId(id2,scene)->state.get();	
		Vector3r shift2=scene->isPeriodic ? Vector3r(scene->cell->Hsize*contact->cellDist.cast<Real>()) : Vector3r::Zero();
		Vector3r shiftVel=scene->isPeriodic ? Vector3r(scene->cell->velGrad*scene->cell->Hsize*contact->cellDist.cast<Real>()) : Vector3r::Zero();
		Vector3r incidentV = geom->getIncidentVel(de1, de2, scene->dt, shift2, shiftVel, /*preventGranularRatcheting*/ nonLin>2 );	
		Vector3r incidentVn = geom->normal.dot(incidentV)*geom->normal; // contact normal velocity
		Vector3r incidentVs = incidentV-incidentVn; // contact shear velocity
		shearIncrement=incidentVs*scene->dt;
	} else { shearIncrement=geom->shearIncrement(); }
	Fs-=ks*shearIncrement;
	// Mohr-Coulomb slip
	Real maxFs2=pow(Fn,2)*pow(phys->tangensOfFrictionAngle,2);
	if(Fs.squaredNorm()>maxFs2) Fs*=sqrt(maxFs2)/Fs.norm();

	// apply forces
	Vector3r f=-phys->normalForce-phys->shearForce; /* should be a reference returned by geom->rotate */ assert(phys->shearForce==Fs); 
	scene->forces.addForce(id1,f);
	scene->forces.addForce(id2,-f);
	scene->forces.addTorque(id1,(geom->radius1-.5*geom->penetrationDepth)*geom->normal.cross(f));
	scene->forces.addTorque(id2,(geom->radius2-.5*geom->penetrationDepth)*geom->normal.cross(f));
}


/******************** Law2_ScGeom_MindlinPhys_Mindlin *********/
CREATE_LOGGER(Law2_ScGeom_MindlinPhys_Mindlin);

void Law2_ScGeom_MindlinPhys_Mindlin::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact){
	const Real& dt = scene->dt; // get time step
	
	Body::id_t id1 = contact->getId1(); // get id body 1
 	Body::id_t id2 = contact->getId2(); // get id body 2

	State* de1 = Body::byId(id1,scene)->state.get();
	State* de2 = Body::byId(id2,scene)->state.get();	

	ScGeom* scg = static_cast<ScGeom*>(ig.get());
	MindlinPhys* phys = static_cast<MindlinPhys*>(ip.get());	

	const shared_ptr<Body>& b1=Body::byId(id1,scene); // not sure here (I need it because I want to know if the body isDynamic or not)
	const shared_ptr<Body>& b2=Body::byId(id2,scene); // not sure here

	bool useDamping=(phys->betan!=0. || phys->betas!=0.);

	// tangential and normal stiffness coefficients, recomputed from betan,betas at every step
	Real cn=0, cs=0;

	/****************/
	/* NORMAL FORCE */
	/****************/
	
	Real uN = scg->penetrationDepth; // get overlapping  
	if (uN<0) {scene->interactions->requestErase(contact->getId1(),contact->getId2()); return;}
	/* Hertz-Mindlin's formulation (PFC) 
	Note that the normal stiffness here is a secant value (so as it is cannot be used in the GSTS)
	In the first place we get the normal force and then we store kn to be passed to the GSTS */
	Real Fn = phys->kno*std::pow(uN,1.5); // normal Force (scalar)
	if (includeAdhesion) {
			Fn -= phys->adhesionForce; // include adhesion force to account for the effect of Van der Waals interactions
			phys->isAdhesive = (Fn<0); // set true the bool to count the number of adhesive contacts
			}
	phys->normalForce = Fn*scg->normal; // normal Force (vector)

	if (calcEnergy){
		Real R=scg->radius1*scg->radius2/(scg->radius1+scg->radius2);
		phys->radius=pow((Fn+(includeAdhesion?phys->adhesionForce:0.))*pow(R,3/2.)/phys->kno,1/3.); // not used anywhere
	}

	/*******************************/
	/* TANGENTIAL NORMAL STIFFNESS */
	/*******************************/
	
	phys->kn = 3./2.*phys->kno*std::pow(uN,0.5); // here we store the value of kn to compute the time step
	
	/******************************/
	/* TANGENTIAL SHEAR STIFFNESS */
	/******************************/
	
	phys->ks = phys->kso*std::pow(uN,0.5); // get tangential stiffness (this is a tangent value, so we can pass it to the GSTS)

	/************************/
	/* DAMPING COEFFICIENTS */
	/************************/
	
	// Inclusion of local damping if requested 
	if (useDamping){
		Real mbar = (!b1->isDynamic() && b2->isDynamic()) ? de2->mass : ((!b2->isDynamic() && b1->isDynamic()) ? de1->mass : (de1->mass*de2->mass / (de1->mass + de2->mass))); // get equivalent mass if both bodies are dynamic, if not set it equal to the one of the dynamic body
		//Real mbar = de1->mass*de2->mass / (de1->mass + de2->mass); // equivalent mass
		Real Cn_crit = 2.*sqrt(mbar*phys->kn); // Critical damping coefficient (normal direction)
		Real Cs_crit = 2.*sqrt(mbar*phys->ks); // Critical damping coefficient (shear direction)
		// Note: to compare with the analytical solution you provide cn and cs directly (since here we used a different method to define c_crit)
		cn = Cn_crit*phys->betan; // Damping normal coefficient
		cs = Cs_crit*phys->betas; // Damping tangential coefficient
		if(phys->kn<0 || phys->ks<0){ cerr<<"Negative stiffness kn="<<phys->kn<<" ks="<<phys->ks<<" for ##"<<b1->getId()<<"+"<<b2->getId()<<", step "<<scene->iter<<endl; }
	}

	/***************/
	/* SHEAR FORCE */
	/***************/
	
	Vector3r& shearElastic = phys->shearElastic; // reference for shearElastic force
	// Define shift to handle periodicity
	Vector3r shift2   = scene->isPeriodic ? Vector3r(                      scene->cell->Hsize *contact->cellDist.cast<Real>()) : Vector3r::Zero();
	Vector3r shiftVel = scene->isPeriodic ? Vector3r((scene->cell->velGrad*scene->cell->Hsize)*contact->cellDist.cast<Real>()) : Vector3r::Zero();
	// 1. Rotate shear force
	shearElastic = scg->rotate(shearElastic);
	Vector3r prev_FsElastic = shearElastic; // save shear force at previous time step
	// 2. Get incident velocity, get shear and normal components
	// FIXME: Concerning the possibility to avoid granular ratcheting, it is not clear how this works in the case of HM. See the thread http://www.mail-archive.com/yade-users@lists.launchpad.net/msg01947.html
	Vector3r incidentV = scg->getIncidentVel(de1, de2, dt, shift2, shiftVel, preventGranularRatcheting);	
	Vector3r incidentVn = scg->normal.dot(incidentV)*scg->normal; // contact normal velocity
	Vector3r incidentVs = incidentV - incidentVn; // contact shear velocity
	// 3. Get shear force (incrementally)
	shearElastic = shearElastic - phys->ks*(incidentVs*dt);


	/**************************************/
	/* VISCOUS DAMPING (Normal direction) */
	/**************************************/
	
	// normal force must be updated here before we apply the Mohr-Coulomb criterion
	if (useDamping){ // get normal viscous component 
		phys->normalViscous = cn*incidentVn;
		// add normal viscous component if damping is included
		phys->normalForce -= phys->normalViscous;
		if (calcEnergy) {normDampDissip += phys->normalViscous.dot(incidentVn*dt);} // calc dissipation of energy due to normal damping
	}

	/*************************************/
	/* SHEAR DISPLACEMENT (elastic only) */
	/*************************************/
	
	Vector3r& us_elastic = phys->usElastic;
	us_elastic = scg->rotate(us_elastic); // rotate vector
	Vector3r prevUs_el = us_elastic; // store previous elastic shear displacement (already rotated)
	us_elastic -= incidentVs*dt; // add shear increment

	/****************************************/
	/* SHEAR DISPLACEMENT (elastic+plastic) */
	/****************************************/
	
	Vector3r& us_total = phys->usTotal;
	us_total = scg->rotate(us_total); // rotate vector
	Vector3r prevUs_tot = us_total; // store previous total shear displacement (already rotated)
	us_total -= incidentVs*dt; // add shear increment NOTE: this vector is not passed into the failure criterion, hence it holds also the plastic part of the shear displacement


	bool noShearDamp = false; // bool to decide whether we need to account for shear damping dissipation or not
	
	/********************/
	/* MOHR-COULOMB law */
	/********************/
	
	phys->shearViscous=Vector3r::Zero(); // reset so that during sliding, the previous values is not there
	if (!includeAdhesion) {
		Real maxFs = Fn*phys->tangensOfFrictionAngle;
		if (shearElastic.squaredNorm() > maxFs*maxFs){
			noShearDamp = true; // no damping is added in the shear direction, hence no need to account for shear damping dissipation
			Real ratio = maxFs/shearElastic.norm();
			shearElastic *= ratio; phys->shearForce = shearElastic; /*store only elastic shear displacement*/ us_elastic*= ratio;
			if (calcEnergy) {frictionDissipation += (us_total-prevUs_tot).dot(shearElastic);} // calculate energy dissipation due to sliding behavior
			}
		else if (useDamping){ // add current contact damping if we do not slide and if damping is requested
			phys->shearViscous = cs*incidentVs; // get shear viscous component
			phys->shearForce = shearElastic - phys->shearViscous;}
		else if (!useDamping) {phys->shearForce = shearElastic;} // update the shear force at the elastic value if no damping is present and if we passed MC
	}
	else { // Mohr-Coulomb formulation adpated due to the presence of adhesion (see Thornton, 1991). FIXME: is this correct?
		Real maxFs = phys->tangensOfFrictionAngle*(phys->adhesionForce+Fn); // adhesionForce already included in normalForce (above)
		if (shearElastic.squaredNorm() > maxFs*maxFs){
			noShearDamp = true; // no damping is added in the shear direction, hence no need to account for shear damping dissipation
			Real ratio = maxFs/shearElastic.norm(); shearElastic *= ratio; phys->shearForce = shearElastic; /*store only elastic shear displacement*/ us_elastic *= ratio;
			if (calcEnergy) {frictionDissipation += (us_total-prevUs_tot).dot(shearElastic);} // calculate energy dissipation due to sliding behavior
			}
		else if (useDamping){ // add current contact damping if we do not slide and if damping is requested
			phys->shearViscous = cs*incidentVs; // get shear viscous component
			phys->shearForce = shearElastic - phys->shearViscous;}
		else if (!useDamping) {phys->shearForce = shearElastic;} // update the shear force at the elastic value if no damping is present and if we passed MC
	}

	/************************/
	/* SHEAR ELASTIC ENERGY */
	/************************/
	
	// NOTE: shear elastic energy calculation must come after the MC criterion, otherwise displacements and forces are not updated
	if (calcEnergy) {
		shearEnergy += (us_elastic-prevUs_el).dot((shearElastic+prev_FsElastic)/2.); // NOTE: no additional energy if we perform sliding since us_elastic and prevUs_el will hold the same value (in fact us_elastic is only keeping the elastic part). We work out the area of the trapezium.
	}

	/**************************************************/
	/* VISCOUS DAMPING (energy term, shear direction) */
	/**************************************************/
	
	if (useDamping){ // get normal viscous component (the shear one is calculated inside Mohr-Coulomb criterion, see above)
		if (calcEnergy) {if (!noShearDamp) {shearDampDissip += phys->shearViscous.dot(incidentVs*dt);}} // calc energy dissipation due to viscous linear damping
	}

	/****************/
	/* APPLY FORCES */
	/****************/
	
	if (!scene->isPeriodic)
		applyForceAtContactPoint(-phys->normalForce - phys->shearForce, scg->contactPoint , id1, de1->se3.position, id2, de2->se3.position);
	else { // in scg we do not wrap particles positions, hence "applyForceAtContactPoint" cannot be used
		Vector3r force = -phys->normalForce - phys->shearForce;
		scene->forces.addForce(id1,force);
		scene->forces.addForce(id2,-force);
		scene->forces.addTorque(id1,(scg->radius1-0.5*scg->penetrationDepth)* scg->normal.cross(force));
		scene->forces.addTorque(id2,(scg->radius2-0.5*scg->penetrationDepth)* scg->normal.cross(force));
	}
	phys->prevNormal = scg->normal;
}



#if 0
void Law2_ScGeom_MindlinPhys_Mindlin::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact, Scene* ncb){
	const Real& dt = scene->dt; // get time step
	
	int id1 = contact->getId1(); // get id body 1
  	int id2 = contact->getId2(); // get id body 2

	State* de1 = Body::byId(id1,ncb)->state.get();
	State* de2 = Body::byId(id2,ncb)->state.get();	

	ScGeom* scg = static_cast<ScGeom*>(ig.get());
	MindlinPhys* phys = static_cast<MindlinPhys*>(ip.get());	


	/*** NORMAL FORCE ****/
	Real uN = scg->penetrationDepth; // get overlapping  
	if (uN<0) {ncb->interactions->requestErase(contact->getId1(),contact->getId2()); return;}
	/*** Hertz-Mindlin's formulation (PFC) ***/
	phys->kn = phys->kno*sqrt(uN); // normal stiffness
	Real Fn = phys->kn*uN; // normal Force (scalar)
	phys->normalForce = Fn*scg->normal; // normal Force (vector)


	/*** SHEAR FORCE ***/
	phys->ks = phys->kso*std::pow(Fn,1/3); // shear stiffness
	Vector3r& trialFs = phys->shearForce;
	Vector3r shiftVel = scene->isPeriodic ? (Vector3r)((scene->cell->velGrad*scene->cell->Hsize)*Vector3r((Real) contact->cellDist[0],(Real) contact->cellDist[1],(Real) contact->cellDist[2])) : Vector3r::Zero();
	Vector3r shearDisp = scg->rotateAndGetShear(trialFs,phys->prevNormal,de1,de2,dt,shiftVel,preventGranularRatcheting);
	trialFs -= phys->ks*shearDisp;
  	
 
	/*** MOHR-COULOMB LAW ***/
	Real maxFs = phys->normalForce.squaredNorm();
	if (trialFs.squaredNorm() > maxFs)
	{Real ratio = sqrt(maxFs)/trialFs.norm(); trialFs *= ratio;}
	

	/*** APPLY FORCES ***/
	if (!scene->isPeriodic)
	applyForceAtContactPoint(-phys->normalForce - trialFs, scg->contactPoint , id1, de1->se3.position, id2, de2->se3.position, ncb);
	else { // in scg we do not wrap particles positions, hence "applyForceAtContactPoint" cannot be used
		Vector3r force = -phys->normalForce - trialFs;
		ncb->forces.addForce(id1,force);
		ncb->forces.addForce(id2,-force);
		ncb->forces.addTorque(id1,(scg->radius1-0.5*scg->penetrationDepth)* scg->normal.cross(force));
		ncb->forces.addTorque(id2,(scg->radius2-0.5*scg->penetrationDepth)* scg->normal.cross(force));
	}
	phys->prevNormal = scg->normal;
}
#endif



