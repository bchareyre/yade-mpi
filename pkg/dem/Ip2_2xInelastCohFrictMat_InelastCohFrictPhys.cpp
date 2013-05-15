#include "Ip2_2xInelastCohFrictMat_InelastCohFrictPhys.hpp"




void Ip2_2xInelastCohFrictMat_InelastCohFrictPhys::go(const shared_ptr<Material>& b1    // InelastCohFrictMat
                                        , const shared_ptr<Material>& b2 // InelastCohFrictMat
                                        , const shared_ptr<Interaction>& interaction)
{
	
	InelastCohFrictMat* sdec1 = static_cast<InelastCohFrictMat*>(b1.get());
	InelastCohFrictMat* sdec2 = static_cast<InelastCohFrictMat*>(b2.get());
	ScGeom6D* geom = YADE_CAST<ScGeom6D*>(interaction->geom.get());

	//Create cohesive interractions only once
	if (setCohesionNow && cohesionDefinitionIteration==-1) cohesionDefinitionIteration=scene->iter;
	if (setCohesionNow && cohesionDefinitionIteration!=-1 && cohesionDefinitionIteration!=scene->iter) {
		cohesionDefinitionIteration = -1;
		setCohesionNow = 0;}

	if (geom) {
		if (!interaction->phys) {
			interaction->phys = shared_ptr<InelastCohFrictPhys>(new InelastCohFrictPhys());
			InelastCohFrictPhys* contactPhysics = YADE_CAST<InelastCohFrictPhys*>(interaction->phys.get());
// 			Real Ea 	= sdec1->young;
// 			Real Eb 	= sdec2->young;
// 			Real Va 	= sdec1->poisson;
// 			Real Vb 	= sdec2->poisson;
			Real Da 	= geom->radius1;
			Real Db 	= geom->radius2;
			Real fa 	= sdec1->frictionAngle;
			Real fb 	= sdec2->frictionAngle;
			//Real Kn = 2.0*Ea*Da*Eb*Db/(Ea*Da+Eb*Db);//harmonic average of two stiffnesses

			// harmonic average of modulus
			Real ETC = 2.0*sdec1->eTC*sdec2->eTC/(sdec1->eTC+sdec2->eTC);
			Real ETT = 2.0*sdec1->eTT*sdec2->eTT/(sdec1->eTT+sdec2->eTT);

			Real EB = 2.0*sdec1->eB*sdec2->eB/(sdec1->eB+sdec2->eB);
			Real GTw = 2.0*sdec1->gTw*sdec2->gTw/(sdec1->gTw+sdec2->gTw);

			contactPhysics->crpT = std::min(sdec1->creepT,sdec2->creepT);
			contactPhysics->crpB = std::min(sdec1->creepB,sdec2->creepB);
			contactPhysics->crpTw = std::min(sdec1->creepTw,sdec2->creepTw);
			//
			
// 			Real Ks;
// 			if (Va && Vb) Ks = 2.0*Ea*Da*Va*Eb*Db*Vb/(Ea*Da*Va+Eb*Db*Vb);//harmonic average of two stiffnesses with ks=V*kn for each sphere
// 			else Ks=0;

			Vector3r segment = (Body::byId(interaction->getId1(),scene)->state->pos) - (Body::byId(interaction->getId2(),scene)->state->pos);
			Real length = segment.norm();
			Real pi = 3.14159265;
			Real area = (pow(std::min(Db, Da),2))*pi;
			Real iG = (pow(std::min(2*Db, 2*Da),4))*pi/32.;
			Real iB = (pow(std::min(2*Db, 2*Da),4))*pi/64.;
			// Ignacio Olmedo-Manich, non size-dependent mechanical params.
			
			contactPhysics->knT = ETT*area/length;
			contactPhysics->knC = ETC*area/length;
			contactPhysics->kt = GTw*iG/length;
			contactPhysics->ks = 12*EB*iB/(pow(length,3));
			contactPhysics->kr = EB*iB/length;
			
			contactPhysics->crpT = std::min(sdec1->creepT,sdec2->creepT);
			contactPhysics->crpB = std::min(sdec1->creepB,sdec2->creepB);
			contactPhysics->crpTw = std::min(sdec1->creepTw,sdec2->creepTw);
			
			contactPhysics->tangensOfFrictionAngle	= std::tan(std::min(fa,fb));
			
			contactPhysics->maxElastB = iB*std::min(sdec1->sigmaB,sdec2->sigmaB);
			contactPhysics->maxElastTw = iB*std::min(sdec1->sigmaTw,sdec2->sigmaTw);
						
			contactPhysics->unldB = std::min(sdec1->unloadB,sdec2->unloadB);
			contactPhysics->unldT = std::min(sdec1->unloadT,sdec2->unloadT);
			contactPhysics->unldTw = std::min(sdec1->unloadTw,sdec2->unloadTw);
			
			contactPhysics->dElT = std::min(sdec1->disElT,sdec2->disElT);
			contactPhysics->dElC = std::min(sdec1->disElC,sdec2->disElC);
			
			contactPhysics->epsMaxT = std::min(sdec1->epsilonMaxT,sdec2->epsilonMaxT);
			contactPhysics->epsMaxC = std::min(sdec1->epsilonMaxC,sdec2->epsilonMaxC);
			
			contactPhysics->phBMax = std::min(sdec1->phiBMax,sdec2->phiBMax);
			contactPhysics->phTwMax = std::min(sdec1->phiTwMax,sdec2->phiTwMax);
			
			if ((setCohesionOnNewContacts || setCohesionNow) && sdec1->isCohesive && sdec2->isCohesive)
			{
				contactPhysics->cohesionBroken = false;
				contactPhysics->normalAdhesion = std::min(sdec1->normalCohesion,sdec2->normalCohesion)*pow(std::min(Db, Da),2);
				contactPhysics->shearAdhesion = std::min(sdec1->shearCohesion,sdec2->shearCohesion)*pow(std::min(Db, Da),2);
				geom->initRotations(*(Body::byId(interaction->getId1(),scene)->state),*(Body::byId(interaction->getId2(),scene)->state));
			}
					
			//contactPhysics->elasticRollingLimit = elasticRollingLimit;
		}
		else {// !isNew, but if setCohesionNow, all contacts are initialized like if they were newly created
			InelastCohFrictPhys* contactPhysics = YADE_CAST<InelastCohFrictPhys*>(interaction->phys.get());
			if ((setCohesionNow && sdec1->isCohesive && sdec2->isCohesive) || contactPhysics->initCohesion)
			{
				contactPhysics->cohesionBroken = false;
				contactPhysics->normalAdhesion = std::min(sdec1->normalCohesion,sdec2->normalCohesion)*pow(std::min(geom->radius2, geom->radius1),2);
				contactPhysics->shearAdhesion = std::min(sdec1->shearCohesion,sdec2->shearCohesion)*pow(std::min(geom->radius2, geom->radius1),2);
				geom->initRotations(*(Body::byId(interaction->getId1(),scene)->state),*(Body::byId(interaction->getId2(),scene)->state));
				contactPhysics->initCohesion=false;
			}
		}
	}
};
YADE_PLUGIN((Ip2_2xInelastCohFrictMat_InelastCohFrictPhys));