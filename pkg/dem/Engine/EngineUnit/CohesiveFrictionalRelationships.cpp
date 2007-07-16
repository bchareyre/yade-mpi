/*************************************************************************
*  Copyright (C) 2007 by Bruno CHAREYRE                                 *
*  bruno.chareyre@hmg.inpg.fr                                        *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"CohesiveFrictionalRelationships.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/CohesiveFrictionalContactInteraction.hpp>
#include<yade/pkg-dem/SDECLinkGeometry.hpp> // FIXME - I can't dispatch by SDECLinkGeometry <-> SpheresContactGeometry !!?
#include<yade/pkg-dem/SDECLinkPhysics.hpp> // FIXME
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>


CohesiveFrictionalRelationships::CohesiveFrictionalRelationships()
{
		normalCohesion = 10000000;
		shearCohesion = 10000000;
		setCohesionNow = false;
		setCohesionOnNewContacts = false;
		cohesionDefinitionIteration = -1;
}


void CohesiveFrictionalRelationships::registerAttributes()
{
	REGISTER_ATTRIBUTE(normalCohesion);
	REGISTER_ATTRIBUTE(shearCohesion);
	REGISTER_ATTRIBUTE(setCohesionNow);
	REGISTER_ATTRIBUTE(setCohesionOnNewContacts);	
}


void CohesiveFrictionalRelationships::go(	  const shared_ptr<PhysicalParameters>& b1 // BodyMacroParameters
					, const shared_ptr<PhysicalParameters>& b2 // BodyMacroParameters
					, const shared_ptr<Interaction>& interaction)
{
	BodyMacroParameters* sdec1 = static_cast<BodyMacroParameters*>(b1.get());
	BodyMacroParameters* sdec2 = static_cast<BodyMacroParameters*>(b2.get());
	SpheresContactGeometry* interactionGeometry = dynamic_cast<SpheresContactGeometry*>(interaction->interactionGeometry.get());	
	
	//Create cohesive interractions only once
	if (setCohesionNow && cohesionDefinitionIteration==-1) {
		cohesionDefinitionIteration=Omega::instance().getCurrentIteration();}
	if (setCohesionNow && cohesionDefinitionIteration!=-1 && cohesionDefinitionIteration!=Omega::instance().getCurrentIteration()) {
		cohesionDefinitionIteration = -1;
		setCohesionNow = 0;}
		
	if(interactionGeometry) // so it is SpheresContactGeometry  - NON PERMANENT LINK
	{
		if(interaction->isNew)
		{
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

			Real Kn = 2*Ea*Da*Eb*Db/(Ea*Da+Eb*Db);//harmonic average of two stiffnesses
			Real Ks = 2*Ea*Da*Va*Eb*Db*Vb/(Ea*Da*Va+Eb*Db*Va);//harmonic average of two stiffnesses with ks=V*kn for each sphere

			contactPhysics->initialKn			= Kn;
			contactPhysics->initialKs			= Ks;
//cerr << "Ks: " <<       contactPhysics->initialKs			<< endl;
			contactPhysics->frictionAngle			= std::min(fa,fb); // FIXME - this is actually a waste of memory space, just like initialKs and initialKn
			contactPhysics->tangensOfFrictionAngle		= std::tan(contactPhysics->frictionAngle);

			if (setCohesionOnNewContacts || setCohesionNow) { 
			contactPhysics->normalAdhesion			= normalCohesion*pow(std::min(interactionGeometry->radius2, interactionGeometry->radius1),2);
			contactPhysics->shearAdhesion			= shearCohesion*pow(std::min(interactionGeometry->radius2, interactionGeometry->radius1),2);;
			}

			contactPhysics->prevNormal 			= interactionGeometry->normal;
			contactPhysics->initialEquilibriumDistance	= Dinit;			

			contactPhysics->kn = contactPhysics->initialKn;
			contactPhysics->ks = contactPhysics->initialKs;
			contactPhysics->equilibriumDistance = contactPhysics->initialEquilibriumDistance;

		}
		else
		{	// FIXME - are those lines necessary ???? what they are doing in fact ???
			CohesiveFrictionalContactInteraction* contactPhysics = YADE_CAST<CohesiveFrictionalContactInteraction*>(interaction->interactionPhysics.get());

			contactPhysics->kn = contactPhysics->initialKn;
			contactPhysics->ks = contactPhysics->initialKs;
			contactPhysics->equilibriumDistance = contactPhysics->initialEquilibriumDistance;
			if (setCohesionNow) { 
			contactPhysics->normalAdhesion			= normalCohesion*pow(std::min(interactionGeometry->radius2, interactionGeometry->radius1),2);
			contactPhysics->shearAdhesion			= shearCohesion*pow(std::min(interactionGeometry->radius2, interactionGeometry->radius1),2);
			//setCohesionNow = false;
			}
		}	
		
	}
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
};
