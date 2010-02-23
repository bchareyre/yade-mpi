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
	Real R = 2*Da*Db/(Da+Db); // harmonic average of the radii
	Real Kno = 2*G*Mathr::Sqrt(2*R)/(3*(1-V)); // coefficient for normal stiffness
	Real Kso = 2*std::pow((std::pow(G,2)*3*(1-V)*R),1/3)/(2-V); // coefficient for shear stiffness
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
	Real dt = Omega::instance().getTimeStep(); // get time step
	
	int id1 = contact->getId1(); // get id body 1
  	int id2 = contact->getId2(); // get id body 2

	State* de1 = Body::byId(id1,ncb)->state.get();
	State* de2 = Body::byId(id2,ncb)->state.get();	

	ScGeom* scg = static_cast<ScGeom*>(ig.get());
	MindlinPhys* phys = static_cast<MindlinPhys*>(ip.get());	


	/*** NORMAL FORCE ***/
	Real uN = scg->penetrationDepth; // get overlapping  
	if (uN<0) {ncb->interactions->requestErase(contact->getId1(),contact->getId2()); return;}
	/*** Hertz-Mindlin's formulation (PFC) ***/
	phys->kn = phys->kno*Mathr::Sqrt(uN); // normal stiffness
	Real Fn = phys->kn*uN; // normal Force (scalar)
	phys->normalForce = Fn*scg->normal; // normal Force (vector)


	/*** SHEAR FORCE ***/
	phys->ks = phys->kso*std::pow(Fn,1/3); // normal stiffness
	Vector3r& trialFs = phys->shearForce;
  	scg->updateShearForce(trialFs, phys->ks, phys->prevNormal, de1, de2, dt, preventGranularRatcheting);

 
	/*** MOHR-COULOMB LAW ***/
	Real maxFs = phys->normalForce.SquaredLength();
	if (trialFs.SquaredLength() > maxFs)
	{Real ratio = Mathr::Sqrt(maxFs)/trialFs.Length(); trialFs *= ratio;}
	phys->shearForce = trialFs; // store shearForce now that is at the actual value


	/*** APPLY FORCES ***/
	applyForceAtContactPoint(-phys->normalForce-trialFs , scg->contactPoint , id1, de1->se3.position, id2, de2->se3.position, ncb);
	phys->prevNormal = scg->normal;
}








      


	
	

