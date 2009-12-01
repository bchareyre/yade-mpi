/*C.W.Boon @ NOV 2009 */

YADE_REQUIRE_FEATURE(abcd);

#include"BasicPM.hpp"
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/DemXDofGeom.hpp>



YADE_PLUGIN((CundallStrackLaw)(Ip2_CSMat_CSMat_CSPhys));


/********************** CundallStrackLaw ****************************/
CREATE_LOGGER(CundallStrackLaw);

void CundallStrackLaw::go(shared_ptr<InteractionGeometry>& ig, shared_ptr<InteractionPhysics>& ip, Interaction* contact, MetaBody* rootBody){
	
	Dem3DofGeom* geom=static_cast<Dem3DofGeom*>(ig.get());
	CSPhys* phys=static_cast<CSPhys*>(ip.get());
	
	/*Normal Force */
	Real displN=geom->displacementN();
	if(displN>0){rootBody->interactions->requestErase(contact->getId1(),contact->getId2()); return; }
	phys->normalForce=phys->kn*displN*geom->normal; //normalForce is a vector
	
	/*ShearForce*/
	Real maxFsSq=phys->normalForce.SquaredLength()*pow(phys->tangensOfFrictionAngle,2);
	Vector3r trialFs=phys->ks*geom->displacementT();
	if(trialFs.SquaredLength()>maxFsSq){ geom->slipToDisplacementTMax(sqrt(maxFsSq)); trialFs*=sqrt(maxFsSq/(trialFs.SquaredLength()));}
	phys->shearForce=trialFs;
	
	applyForceAtContactPoint(phys->normalForce+trialFs,geom->contactPoint,contact->getId1(),geom->se31.position,contact->getId2(),geom->se32.position,rootBody);
}
/********************CundallStrackLaw <end> ************************/



/*****************************Ip2_CSMat_CSMat_CSPhys********************************/
CREATE_LOGGER(Ip2_CSMat_CSMat_CSPhys);

void Ip2_CSMat_CSMat_CSPhys::go(const shared_ptr<PhysicalParameters>& b1, const shared_ptr<PhysicalParameters>& b2 , const shared_ptr<Interaction>& interaction)
{

		
		if(interaction->interactionPhysics) return;
		//it could be other interactions by using if-else statements
		Dem3DofGeom* d3dg=dynamic_cast<Dem3DofGeom*>(interaction->interactionGeometry.get());
		assert(d3dg);

			//const shared_ptr<CSMat>& csmat1=YADE_PTR_CAST<CSMat>(b1);
			//const shared_ptr<CSMat>& csmat2=YADE_PTR_CAST<CSMat>(b2);

			const shared_ptr<BodyMacroParameters>& sdec1 = YADE_PTR_CAST<BodyMacroParameters>(b1);
			const shared_ptr<BodyMacroParameters>& sdec2 = YADE_PTR_CAST<BodyMacroParameters>(b2);
			
			shared_ptr<CSPhys> contactPhysics(new CSPhys()); //what if I declared it as constant?
			
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
			Vector3r normal=d3dg->normal;		  			
			Real Kn = 2*Ea*Da*Eb*Db/(Ea*Da+Eb*Db);//harmonic average of two stiffnesses
			Real Ks = 2*Ea*Da*Va*Eb*Db*Vb/(Ea*Da*Va+Eb*Db*Va);//harmonic average of two stiffnesses with ks=V*kn for each sphere

			/* Pass values calculated from above to CSPhys */
			contactPhysics->kn = Kn;
			contactPhysics->ks = Ks;
			contactPhysics->frictionAngle			= std::min(fa,fb); 
			contactPhysics->tangensOfFrictionAngle		= std::tan(contactPhysics->frictionAngle); 
			contactPhysics->prevNormal 			= normal;
			

}
/*****************************Ip2_CSMat_CSMat_CSPhys <end>********************************/




