// 2010 © Chiara Modenese <c.modenese@gmail.com> 


#include"HertzMindlin.hpp"
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>


YADE_PLUGIN((MindlinPhys)(Ip2_FrictMat_FrictMat_MindlinPhys)(Law2_ScGeom_MindlinPhys_Mindlin));


/******************** MindlinPhys *****************************/
CREATE_LOGGER(MindlinPhys);

MindlinPhys::~MindlinPhys(){}; // destructor


/******************** Ip2_FrictMat_FrictMat_MindlinPhys *******/
CREATE_LOGGER(Ip2_FrictMat_FrictMat_MindlinPhys);

void Ip2_FrictMat_FrictMat_MindlinPhys::go(const shared_ptr<Material>& b1,const shared_ptr<Material>& b2, const shared_ptr<Interaction>& interaction){
	if(interaction->interactionPhysics) return; // no updates of an already existing contact necessary
	shared_ptr<MindlinPhys> mindlinPhys(new MindlinPhys());
	interaction->interactionPhysics = mindlinPhys;
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
	ScGeom* scg = YADE_CAST<ScGeom*>(interaction->interactionGeometry.get());		
	Real Da = scg->radius1; 
	Real Db = scg->radius2; 
	Vector3r normal=scg->normal; 


	/* calculate stiffness coefficients */
	Real Ga = Ea/(2*(1+Va));
	Real Gb = Eb/(2*(1+Vb));
	Real G = (Ga+Gb)/2; // average of shear modulus
	Real V = (Va+Vb)/2; // average of poisson's ratio
	Real E = Ea*Eb/((1.-std::pow(Va,2.0))*Eb+(1.-std::pow(Vb,2.0))*Ea); // Young modulus
	Real R = Da*Db/(Da+Db); // equivalent radius
	Real Kno = 4./3.*E*std::pow(R,0.5); // coefficient for normal stiffness
	Real Kso = 2*sqrt(4*R)*G/(2-V); // coefficient for shear stiffness
	Real frictionAngle = std::min(fa,fb);


	/* pass values calculated from above to MindlinPhys */
	mindlinPhys->tangensOfFrictionAngle = std::tan(frictionAngle); 
	mindlinPhys->prevNormal = scg->normal;
	mindlinPhys->kno = Kno; // this is just a coeff
	mindlinPhys->kso = Kso; // this is just a coeff
}


/******************** Law2_ScGeom_MindlinPhys_Mindlin *********/
CREATE_LOGGER(Law2_ScGeom_MindlinPhys_Mindlin);

void Law2_ScGeom_MindlinPhys_Mindlin::go(shared_ptr<InteractionGeometry>& ig, shared_ptr<InteractionPhysics>& ip, Interaction* contact, Scene* ncb){
	const Real& dt = scene->dt; // get time step
	
	int id1 = contact->getId1(); // get id body 1
  	int id2 = contact->getId2(); // get id body 2

	State* de1 = Body::byId(id1,ncb)->state.get();
	State* de2 = Body::byId(id2,ncb)->state.get();	

	ScGeom* scg = static_cast<ScGeom*>(ig.get());
	MindlinPhys* phys = static_cast<MindlinPhys*>(ip.get());	


	/*** NORMAL FORCE ***/
	Real uN = scg->penetrationDepth; // get overlapping  
	if (uN<0) {ncb->interactions->requestErase(contact->getId1(),contact->getId2()); return;}
	/* Hertz-Mindlin's formulation (PFC)
	Note that the normal stiffness here is a secant value (so as it is cannot be used in the GSTS)
	In the first place we get the normal force and then we store kn to be passed to the GSTS */
	Real Fn = phys->kno*std::pow(uN,1.5);// normal Force (scalar)
	phys->normalForce = Fn*scg->normal; // normal Force (vector)
	phys->kn = 3./2.*phys->kno*std::pow(uN,0.5); // here we store the value of kn to compute the time step


	/*** SHEAR FORCE ***/
	phys->ks = phys->kso*std::pow(uN,0.5); // get tangential stiffness (this is a tangent value, so we can pass it to the GSTS)
	Vector3r& trialFs = phys->shearForce;
	// define shift to handle periodicity
	Vector3r shiftVel = scene->isPeriodic ? (Vector3r)((scene->cell->velGrad*scene->cell->Hsize)*Vector3r((Real) contact->cellDist[0],(Real) contact->cellDist[1],(Real) contact->cellDist[2])) : Vector3r::Zero();
  	Vector3r dus =scg->rotateAndGetShear(trialFs, phys->prevNormal, de1, de2, dt, shiftVel, preventGranularRatcheting);
	//Linear elasticity giving "trial" shear force
	trialFs -= phys->ks*dus;

 
	/*** MOHR-COULOMB LAW ***/
	Real maxFs = phys->normalForce.squaredNorm()*std::pow(phys->tangensOfFrictionAngle,2);
	if (trialFs.squaredNorm() > maxFs)
	{Real ratio = Mathr::Sqrt(maxFs)/trialFs.norm(); trialFs *= ratio;}


	/*** APPLY FORCES ***/
	if (!scene->isPeriodic)
	applyForceAtContactPoint(-phys->normalForce-trialFs , scg->contactPoint , id1, de1->se3.position, id2, de2->se3.position, ncb);
	else { // in scg we do not wrap particles positions, hence "applyForceAtContactPoint" cannot be used
		Vector3r force = -phys->normalForce-trialFs;
		ncb->forces.addForce(id1,force);
		ncb->forces.addForce(id2,-force);
		ncb->forces.addTorque(id1,(scg->radius1-0.5*scg->penetrationDepth)* scg->normal.cross(force));
		ncb->forces.addTorque(id2,(scg->radius2-0.5*scg->penetrationDepth)* scg->normal.cross(force));
	}
	phys->prevNormal = scg->normal;
}








      


	
	

