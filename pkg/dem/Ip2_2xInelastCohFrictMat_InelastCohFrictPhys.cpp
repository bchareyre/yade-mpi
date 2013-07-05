/*************************************************************************
*  Copyright (C) 2012 by Ignacio Olmedo nolmedo.manich@gmail.com         *
*  Copyright (C) 2012 by François Kneib   francois.kneib@gmail.com       *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#include "Ip2_2xInelastCohFrictMat_InelastCohFrictPhys.hpp"
#include<yade/pkg/dem/ScGeom.hpp>



void Ip2_2xInelastCohFrictMat_InelastCohFrictPhys::go(const shared_ptr<Material>& b1    // InelastCohFrictMat
                                        , const shared_ptr<Material>& b2 // InelastCohFrictMat
                                        , const shared_ptr<Interaction>& interaction)
{
	
	InelastCohFrictMat* sdec1 = static_cast<InelastCohFrictMat*>(b1.get());
	InelastCohFrictMat* sdec2 = static_cast<InelastCohFrictMat*>(b2.get());
	ScGeom6D* geom = YADE_CAST<ScGeom6D*>(interaction->geom.get());
	
	//FIXME : non cohesive contact are not implemented, it would be useful to use setCohesionNow, setCohesionOnNewContacts etc ...

	if (geom) {
		if (!interaction->phys) {
			interaction->phys = shared_ptr<InelastCohFrictPhys>(new InelastCohFrictPhys());
			InelastCohFrictPhys* contactPhysics = YADE_CAST<InelastCohFrictPhys*>(interaction->phys.get());
			Real pi = 3.14159265;
			Real r1 	= geom->radius1;
			Real r2 	= geom->radius2;
			Real f1 	= sdec1->frictionAngle;
			Real f2 	= sdec2->frictionAngle;
			
			contactPhysics->tangensOfFrictionAngle	= tan(min(f1,f2));
			
			// harmonic average of modulus
			contactPhysics->knC = 2.0*sdec1->compressionModulus*r1*sdec2->compressionModulus*r2/(sdec1->compressionModulus*r1+sdec2->compressionModulus*r2);
			contactPhysics->knT = 2.0*sdec1->tensionModulus*r1*sdec2->tensionModulus*r2/(sdec1->tensionModulus*r1+sdec2->tensionModulus*r2);
			contactPhysics->ks = 2.0*sdec1->shearModulus*r1*sdec2->shearModulus*r2/(sdec1->shearModulus*r1+sdec2->shearModulus*r2); 
			
			// harmonic average of coeficients for bending and twist coeficients
			Real AlphaKr = 2.0*sdec1->alphaKr*sdec2->alphaKr/(sdec1->alphaKr+sdec2->alphaKr);
			Real AlphaKtw = 2.0*sdec1->alphaKtw*sdec2->alphaKtw/(sdec1->alphaKtw+sdec2->alphaKtw);
			
			contactPhysics->kr = r1*r2*contactPhysics->ks*AlphaKr;
			contactPhysics->ktw = r1*r2*contactPhysics->ks*AlphaKtw;
			
			contactPhysics->kTCrp	= contactPhysics->knT*min(sdec1->creepTension,sdec2->creepTension);
			contactPhysics->kRCrp	= contactPhysics->kr*min(sdec1->creepBending,sdec2->creepBending);
			contactPhysics->kTwCrp	= contactPhysics->ktw*min(sdec1->creepTwist,sdec2->creepTwist);
			
			contactPhysics->kRUnld =  contactPhysics->kr*min(sdec1->unloadBending,sdec2->unloadBending);
			contactPhysics->kTUnld =  contactPhysics->knT*min(sdec1->unloadTension,sdec2->unloadTension);
			contactPhysics->kTwUnld = contactPhysics->ktw*min(sdec1->unloadTwist,sdec2->unloadTwist);

			contactPhysics->maxElC =  min(sdec1->sigmaCompression,sdec2->sigmaCompression)*pow(min(r2, r1),2);
			contactPhysics->maxElT =  min(sdec1->sigmaTension,sdec2->sigmaTension)*pow(min(r2, r1),2);
			contactPhysics->maxElB =  min(sdec1->nuBending,sdec2->nuBending)*pow(min(r2, r1),3);
			contactPhysics->maxElTw = min(sdec1->nuTwist,sdec2->nuTwist)*pow(min(r2, r1),3);
								
			contactPhysics->shearAdhesion = min(sdec1->shearCohesion,sdec2->shearCohesion)*pow(min(r1, r2),2);
			
			contactPhysics->maxExten = min(sdec1->epsilonMaxTension*r1,sdec2->epsilonMaxTension*r2);
			contactPhysics->maxContract = min(sdec1->epsilonMaxCompression*r1,sdec2->epsilonMaxCompression*r2);
			
			contactPhysics->maxBendMom = min(sdec1->etaMaxBending,sdec2->etaMaxBending)*pow(min(r2, r1),3);
			contactPhysics->maxTwist = 2*pi*min(sdec1->etaMaxTwist,sdec2->etaMaxTwist);
		}
	}
};
YADE_PLUGIN((Ip2_2xInelastCohFrictMat_InelastCohFrictPhys));