/*************************************************************************
*  Copyright (C) 2008 by Jérôme DURIEZ                                   *
*  duriez@geo.hmg.inpg.fr                                                *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"CL1Relationships.hpp"
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/pkg-dem/ContactLaw1Interaction.hpp>
// #include<yade/pkg-dem/SDECLinkGeometry.hpp> // FIXME - I can't dispatch by SDECLinkGeometry <-> ScGeom !!?
// #include<yade/pkg-dem/SDECLinkPhysics.hpp> // FIXME
#include<yade/pkg-dem/CohesiveFrictionalMat.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>


CL1Relationships::CL1Relationships()
{
	betaR = 0.12;
	setCohesionNow = false;
	setCohesionOnNewContacts = false;
	cohesionDefinitionIteration = -1; 

//		elasticRollingLimit = ;
}



//
//
//
/// Big WHAT THE ....? this code below is duplicated THREE times due to some weird IFs !
/// need to FIXME that.
/// but from all my testing it works currently. / janek
//
//

void CL1Relationships::go(	  const shared_ptr<PhysicalParameters>& b1 // CohesiveFrictionalMat
					, const shared_ptr<PhysicalParameters>& b2 // CohesiveFrictionalMat
					, const shared_ptr<Interaction>& interaction)
{
	CohesiveFrictionalMat* sdec1 = static_cast<CohesiveFrictionalMat*>(b1.get());
	CohesiveFrictionalMat* sdec2 = static_cast<CohesiveFrictionalMat*>(b2.get());
	ScGeom* interactionGeometry = YADE_CAST<ScGeom*>(interaction->interactionGeometry.get());
	
	//Create cohesive interractions only once
	if (setCohesionNow && cohesionDefinitionIteration==-1) {
		cohesionDefinitionIteration=Omega::instance().getCurrentIteration();}
	if (setCohesionNow && cohesionDefinitionIteration!=-1 && cohesionDefinitionIteration!=Omega::instance().getCurrentIteration()) {
		cohesionDefinitionIteration = -1;
		setCohesionNow = 0;}
	
	
	if(interactionGeometry) // so it is ScGeom  - NON PERMANENT LINK
	{
		if(!interaction->interactionPhysics)
		{
//std::cerr << " isNew, id1: " << interaction->getId1() << " id2: " << interaction->getId2()  << "\n";
			interaction->interactionPhysics = shared_ptr<ContactLaw1Interaction>(new ContactLaw1Interaction());
			ContactLaw1Interaction* contactPhysics = YADE_CAST<ContactLaw1Interaction*>(interaction->interactionPhysics.get());

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
			Real Kr = betaR*std::pow((Da+Db)/2.0,2)*Ks;

			contactPhysics->initialKn			= Kn;
			contactPhysics->initialKs			= Ks;
//cerr << "Ks: " <<       contactPhysics->initialKs			<< endl;
			contactPhysics->frictionAngle			= std::min(fa,fb); // FIXME - this is actually a waste of memory space, just like initialKs and initialKn
			contactPhysics->tangensOfFrictionAngle		= std::tan(contactPhysics->frictionAngle);
			contactPhysics->forMaxMoment		= 1.0*(Da+Db)/2.0;	// 1.0 corresponding to ethaR which I don't know exactly where to define as a parameter...

			if ((setCohesionOnNewContacts || setCohesionNow) && sdec1->isCohesive && sdec2->isCohesive) 
			{
			
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
			ContactLaw1Interaction* contactPhysics = YADE_CAST<ContactLaw1Interaction*>(interaction->interactionPhysics.get());

			contactPhysics->kn = contactPhysics->initialKn;
			contactPhysics->ks = contactPhysics->initialKs;
			contactPhysics->equilibriumDistance = contactPhysics->initialEquilibriumDistance;

			if (setCohesionNow && sdec1->isCohesive && sdec2->isCohesive) 
			{ 
				//setCohesionNow = false;

			contactPhysics->initialOrientation1 = sdec1->se3.orientation;
			contactPhysics->initialOrientation2 = sdec2->se3.orientation;
			contactPhysics->initialPosition1    = sdec1->se3.position;
			contactPhysics->initialPosition2    = sdec2->se3.position;
			Real Da 	= interactionGeometry->radius1; // FIXME - multiply by factor of sphere interaction distance (so sphere interacts at bigger range that its geometrical size)
			Real Db 	= interactionGeometry->radius2; // FIXME - as above
			Real Kr = betaR*std::pow((Da+Db)/2.0,2)*contactPhysics->ks;
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
YADE_PLUGIN((CL1Relationships));

YADE_REQUIRE_FEATURE(PHYSPAR);
