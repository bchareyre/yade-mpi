//CWBoon@2009 booncw@hotmail.com //

#include"CundallStrack.hpp"
#include<yade/core/Scene.hpp>
#include<yade/pkg-dem/DemXDofGeom.hpp>
//! tested in scripts/test/CundallStrack.py

YADE_PLUGIN((Law2_Dem3Dof_CSPhys_CundallStrack)(Ip2_2xFrictMat_CSPhys)(CSPhys));



/********************** Law2_Dem3DofGeom_RockPMPhys_Rpm ****************************/
CREATE_LOGGER(Law2_Dem3Dof_CSPhys_CundallStrack);

void Law2_Dem3Dof_CSPhys_CundallStrack::go(shared_ptr<InteractionGeometry>& ig, shared_ptr<InteractionPhysics>& ip, Interaction* contact, Scene* rootBody){
	Dem3DofGeom* geom=static_cast<Dem3DofGeom*>(ig.get());
	CSPhys* phys=static_cast<CSPhys*>(ip.get());
	
	/*NormalForce */
	Real displN=geom->displacementN();
	if (displN>0){rootBody->interactions->requestErase(contact->getId1(),contact->getId2()); return;}
	phys->normalForce=phys->kn*displN*geom->normal;

	/*ShearForce*/
	Real maxFsSq=phys->normalForce.squaredNorm()*pow(phys->tanFrictionAngle,2);
	Vector3r trialFs=phys->ks*geom->displacementT();
	if(trialFs.squaredNorm()>maxFsSq){ geom->slipToDisplacementTMax(sqrt(maxFsSq)); 
	trialFs*=sqrt(maxFsSq/(trialFs.squaredNorm()));}
	phys->shearForce = trialFs;

	applyForceAtContactPoint(phys->normalForce + trialFs, geom->contactPoint, contact->getId1(), geom->se31.position, contact->getId2(), geom->se32.position, rootBody);
	return;				
	
}

CREATE_LOGGER(Ip2_2xFrictMat_CSPhys);

void Ip2_2xFrictMat_CSPhys::go(const shared_ptr<Material>& b1, const shared_ptr<Material>& b2, const shared_ptr<Interaction>& interaction){
	
	if(interaction->interactionPhysics) return; 

	Dem3DofGeom* d3dg=dynamic_cast<Dem3DofGeom*>(interaction->interactionGeometry.get());
			
	assert(d3dg);

	const shared_ptr<FrictMat>& sdec1 = YADE_PTR_CAST<FrictMat>(b1);
	const shared_ptr<FrictMat>& sdec2 = YADE_PTR_CAST<FrictMat>(b2);
			
	shared_ptr<CSPhys> contactPhysics(new CSPhys()); 
			
	/* From interaction physics */
	Real Ea 	= sdec1->young;
	Real Eb 	= sdec2->young;
	Real Va 	= sdec1->poisson;
	Real Vb 	= sdec2->poisson;
	Real fa 	= sdec1->frictionAngle;
	Real fb 	= sdec2->frictionAngle;

	/* From interaction geometry */
	Real Da=d3dg->refR1>0?d3dg->refR1:d3dg->refR2; 
	Real Db=d3dg->refR2>0?d3dg->refR2:d3dg->refR1; 		  			
	Real Kn = 2*Ea*Da*Eb*Db/(Ea*Da+Eb*Db);//harmonic average of two stiffnesses
	Real Ks = 2*Ea*Da*Va*Eb*Db*Vb/(Ea*Da*Va+Eb*Db*Va);//harmonic average of two stiffnesses with ks=V*kn for each sphere

	/* Pass values calculated from above to CSPhys */
	contactPhysics->kn = Kn;
	contactPhysics->ks = Ks;
	contactPhysics->frictionAngle			= std::min(fa,fb); 
	contactPhysics->tanFrictionAngle		= std::tan(contactPhysics->frictionAngle); 
			
	interaction->interactionPhysics = contactPhysics;
}

CSPhys::~CSPhys(){};

