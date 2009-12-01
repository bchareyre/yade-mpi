/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre <bruno.chareyre@imag.fr>         *
*  Copyright (C) 2008 by Janek Kozicki <cosurgi@berlios.de>              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"CohesiveFrictionalRelationships.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/CohesiveFrictionalContactInteraction.hpp>
//#include<yade/pkg-dem/SDECLinkGeometry.hpp> // FIXME - I can't dispatch by SDECLinkGeometry <-> SpheresContactGeometry !!?
//#include<yade/pkg-dem/SDECLinkPhysics.hpp> // FIXME
#include<yade/pkg-dem/CohesiveFrictionalBodyParameters.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>


CohesiveFrictionalRelationships::CohesiveFrictionalRelationships()
{
		normalCohesion = 10000000;
		shearCohesion = 10000000;
		setCohesionNow = false;
		setCohesionOnNewContacts = false;
		cohesionDefinitionIteration = -1; 

//		elasticRollingLimit = ;
}



//
//
//
/// Big WHAT THE FUCK? this code below is duplicated THREE times due to some weird IFs !
/// need to FIXME that.
/// but from all my testing it works currently. / janek
//
//

void CohesiveFrictionalRelationships::go(	  const shared_ptr<PhysicalParameters>& b1 // CohesiveFrictionalBodyParameters
					, const shared_ptr<PhysicalParameters>& b2 // CohesiveFrictionalBodyParameters
					, const shared_ptr<Interaction>& interaction)
{
	CohesiveFrictionalBodyParameters* sdec1 = static_cast<CohesiveFrictionalBodyParameters*>(b1.get());
	CohesiveFrictionalBodyParameters* sdec2 = static_cast<CohesiveFrictionalBodyParameters*>(b2.get());
	SpheresContactGeometry* interactionGeometry = YADE_CAST<SpheresContactGeometry*>(interaction->interactionGeometry.get());
	
	//Create cohesive interractions only once
	if (setCohesionNow && cohesionDefinitionIteration==-1) {
		cohesionDefinitionIteration=Omega::instance().getCurrentIteration();}
	if (setCohesionNow && cohesionDefinitionIteration!=-1 && cohesionDefinitionIteration!=Omega::instance().getCurrentIteration()) {
		cohesionDefinitionIteration = -1;
		setCohesionNow = 0;}
	
	
	if(interactionGeometry) // so it is SpheresContactGeometry  - NON PERMANENT LINK
	{
		if(!interaction->interactionPhysics)
		{
//std::cerr << " isNew, id1: " << interaction->getId1() << " id2: " << interaction->getId2()  << "\n";
			interaction->interactionPhysics = shared_ptr<CohesiveFrictionalContactInteraction>(new CohesiveFrictionalContactInteraction());
			CohesiveFrictionalContactInteraction* contactPhysics = YADE_CAST<CohesiveFrictionalContactInteraction*>(interaction->interactionPhysics.get());

			Real Ea 	= sdec1->young;
			Real Eb 	= sdec2->young;
			Real Va 	= sdec1->poisson;
			Real Vb 	= sdec2->poisson;
			Real Da 	= interactionGeometry->radius1; // FIXME - multiply by factor of sphere interaction distance (so sphere interacts at bigger range that its geometrical size)
			Real Db 	= interactionGeometry->radius2; // FIXME - as above
			Real fa 	= sdec1->frictionAngle;
			Real fb 	= sdec2->frictionAngle;

			//Real Eab	= 2*Ea*Eb/(Ea+Eb);
			//Real Vab	= 2*Va*Vb/(Va+Vb);

			Real Dinit 	= Da+Db; 			// FIXME - is it just a sum?
			//Real Sinit 	= Mathr::PI * std::pow( std::min(Da,Db) , 2);

			Real Kn = 2.0*Ea*Da*Eb*Db/(Ea*Da+Eb*Db);//harmonic average of two stiffnesses
			Real Ks = 2.0*Ea*Da*Va*Eb*Db*Vb/(Ea*Da*Va+Eb*Db*Va);//harmonic average of two stiffnesses with ks=V*kn for each sphere

			// Jean-Patrick Plassiard, Noura Belhaine, Frederic
			// Victor Donze, "Calibration procedure for spherical
			// discrete elements using a local moemnt law".
			Real Kr = Da*Db*Ks*2.0; // just like "2.0" above - it's an arbitrary parameter

			contactPhysics->initialKn			= Kn;
			contactPhysics->initialKs			= Ks;
//cerr << "Ks: " <<       contactPhysics->initialKs			<< endl;
			contactPhysics->frictionAngle			= std::min(fa,fb); // FIXME - this is actually a waste of memory space, just like initialKs and initialKn
			contactPhysics->tangensOfFrictionAngle		= std::tan(contactPhysics->frictionAngle);

			if ((setCohesionOnNewContacts || setCohesionNow) && sdec1->isCohesive && sdec2->isCohesive) 
			{
				contactPhysics->cohesionBroken = false;
				contactPhysics->normalAdhesion			= normalCohesion*pow(std::min(Db, Da),2);
				contactPhysics->shearAdhesion			= shearCohesion*pow(std::min(Db, Da),2);;
			
				// FIXME - not sure: do I need to repeat it here [1] ?
				contactPhysics->initialOrientation1	= sdec1->se3.orientation;
				contactPhysics->initialOrientation2	= sdec2->se3.orientation;
				contactPhysics->initialPosition1    = sdec1->se3.position;
				contactPhysics->initialPosition2    = sdec2->se3.position;
				contactPhysics->kr = Kr;
				contactPhysics->initialContactOrientation.Align(Vector3r(1.0,0.0,0.0),interactionGeometry->normal);
				contactPhysics->currentContactOrientation = contactPhysics->initialContactOrientation;
				contactPhysics->orientationToContact1   = contactPhysics->initialOrientation1.Conjugate() * contactPhysics->initialContactOrientation;
				contactPhysics->orientationToContact2	= contactPhysics->initialOrientation2.Conjugate() * contactPhysics->initialContactOrientation;


//if((interaction->getId1()==7 && interaction->getId2()==12)||(interaction->getId1()==12 && interaction->getId2()==7)){
//Vector3r axis0;
//Real angle0;
//contactPhysics->initialRelativeOrientation.ToAxisAngle(axis0,angle0);
//std::cout << "-----------------------\n"
//                                               << " ax0: " <<  axis0[0] << " " << axis0[1] << " " << axis0[2] << " an0: " << angle0 << "\n";
//}

				//contactPhysics->elasticRollingLimit = elasticRollingLimit;
			}

			contactPhysics->prevNormal 			= interactionGeometry->normal;
			contactPhysics->initialEquilibriumDistance	= Dinit;			

			contactPhysics->kn = contactPhysics->initialKn;
			contactPhysics->ks = contactPhysics->initialKs;
			contactPhysics->equilibriumDistance = contactPhysics->initialEquilibriumDistance;

			// FIXME - or here [1] ?
			contactPhysics->initialOrientation1	= sdec1->se3.orientation;
			contactPhysics->initialOrientation2	= sdec2->se3.orientation;
			contactPhysics->initialPosition1    = sdec1->se3.position;
			contactPhysics->initialPosition2    = sdec2->se3.position;
			contactPhysics->kr = Kr;
			contactPhysics->initialContactOrientation.Align(Vector3r(1.0,0.0,0.0),interactionGeometry->normal);
			contactPhysics->currentContactOrientation = contactPhysics->initialContactOrientation;
			contactPhysics->orientationToContact1   = contactPhysics->initialOrientation1.Conjugate() * contactPhysics->initialContactOrientation;
			contactPhysics->orientationToContact2	= contactPhysics->initialOrientation2.Conjugate() * contactPhysics->initialContactOrientation;
			//contactPhysics->elasticRollingLimit = elasticRollingLimit;
			//
		}
		else // !isNew
		{	
			// FIXME - are those lines necessary ???? what they are doing in fact ???
			// ANSWER - they are used when you setCohesionNow (contact isNew not)
			CohesiveFrictionalContactInteraction* contactPhysics = YADE_CAST<CohesiveFrictionalContactInteraction*>(interaction->interactionPhysics.get());

			contactPhysics->kn = contactPhysics->initialKn;
			contactPhysics->ks = contactPhysics->initialKs;
			contactPhysics->equilibriumDistance = contactPhysics->initialEquilibriumDistance;

			if (setCohesionNow && sdec1->isCohesive && sdec2->isCohesive) 
			{ 
				contactPhysics->cohesionBroken = false;
				contactPhysics->normalAdhesion			= normalCohesion*pow(std::min(interactionGeometry->radius2, interactionGeometry->radius1),2);
				contactPhysics->shearAdhesion			= shearCohesion*pow(std::min(interactionGeometry->radius2, interactionGeometry->radius1),2);
				//setCohesionNow = false;

			contactPhysics->initialOrientation1 = sdec1->se3.orientation;
			contactPhysics->initialOrientation2 = sdec2->se3.orientation;
			contactPhysics->initialPosition1    = sdec1->se3.position;
			contactPhysics->initialPosition2    = sdec2->se3.position;
			Real Da 	= interactionGeometry->radius1; // FIXME - multiply by factor of sphere interaction distance (so sphere interacts at bigger range that its geometrical size)
			Real Db 	= interactionGeometry->radius2; // FIXME - as above
			Real Kr = Da*Db*contactPhysics->ks*2.0; // just like "2.0" above - it's an arbitrary parameter
			contactPhysics->kr = Kr;
			contactPhysics->initialContactOrientation.Align(Vector3r(1.0,0.0,0.0),interactionGeometry->normal);
			contactPhysics->currentContactOrientation = contactPhysics->initialContactOrientation;
			contactPhysics->orientationToContact1   = contactPhysics->initialOrientation1.Conjugate() * contactPhysics->initialContactOrientation;
			contactPhysics->orientationToContact2	= contactPhysics->initialOrientation2.Conjugate() * contactPhysics->initialContactOrientation;
//Vector3r axis0;
//Real angle0;
//contactPhysics->initialRelativeOrientation.ToAxisAngle(axis0,angle0);
//std::cout << "id1: " << interaction->getId1() << " id2: " << interaction->getId2() << " | "
//                                               << " ax0: " <<  axis0[0] << " " << axis0[1] << " " << axis0[2] << ", an0: " << angle0 << "\n";
				//contactPhysics->elasticRollingLimit = elasticRollingLimit;
			}
		}	
		
	}
#if 0
	else   // this is PERMANENT LINK because previous dynamic_cast failed, dispatcher should do this job
	{
		SDECLinkGeometry* sdecLinkGeometry =  dynamic_cast<SDECLinkGeometry*>(interaction->interactionGeometry.get());
		if (sdecLinkGeometry)
		{		
			SDECLinkPhysics* linkPhysics = static_cast<SDECLinkPhysics*>(interaction->interactionPhysics.get());
	//		linkPhysics->frictionAngle 		= ?? //FIXME - uninitialized 
			linkPhysics->kn 			= linkPhysics->initialKn;
			linkPhysics->ks 			= linkPhysics->initialKs;
			linkPhysics->equilibriumDistance 	= linkPhysics->initialEquilibriumDistance;
		}
	}
#endif
};
YADE_PLUGIN((CohesiveFrictionalRelationships));

YADE_REQUIRE_FEATURE(PHYSPAR);

