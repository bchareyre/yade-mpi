/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"MacroMicroElasticRelationships.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
//#include<yade/pkg-dem/SDECLinkGeometry.hpp> // FIXME - I can't dispatch by SDECLinkGeometry <-> SpheresContactGeometry !!?
//#include<yade/pkg-dem/SDECLinkPhysics.hpp> // FIXME
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/Tetra.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>


MacroMicroElasticRelationships::MacroMicroElasticRelationships()
{
	alpha 	= 2.5;
	beta 	= 2.0;
	gamma 	= 2.65;
}



void MacroMicroElasticRelationships::go(	  const shared_ptr<PhysicalParameters>& b1 // BodyMacroParameters
					, const shared_ptr<PhysicalParameters>& b2 // BodyMacroParameters
					, const shared_ptr<Interaction>& interaction)
{
	BodyMacroParameters* sdec1 = static_cast<BodyMacroParameters*>(b1.get());
	BodyMacroParameters* sdec2 = static_cast<BodyMacroParameters*>(b2.get());
	SpheresContactGeometry* interactionGeometry = YADE_CAST<SpheresContactGeometry*>(interaction->interactionGeometry.get());
	
	if(interactionGeometry) // so it is SpheresContactGeometry  - NON PERMANENT LINK
	{

/* OLD VERSION  this is a LinearContactModel, different class, model different that MicroMacroElasticRelationships
another would be HerzMindlinContactModel

		shared_ptr<ElasticContactInteraction> contactPhysics;
		
		if ( interaction->isNew)
		{
			interaction->interactionPhysics = shared_ptr<ElasticContactInteraction>(new ElasticContactInteraction());
			contactPhysics = YADE_PTR_CAST<ElasticContactInteraction>(interaction->interactionPhysics);
			
			contactPhysics->initialKn			= 2*(sdec1->kn*sdec2->kn)/(sdec1->kn+sdec2->kn);
			contactPhysics->initialKs			= 2*(sdec1->ks*sdec2->ks)/(sdec1->ks+sdec2->ks);
			contactPhysics->prevNormal 			= interactionGeometry->normal;
			contactPhysics->initialEquilibriumDistance	= interactionGeometry->radius1+interactionGeometry->radius2;
		}
		else
			contactPhysics = YADE_PTR_CAST<ElasticContactInteraction>(interaction->interactionPhysics);
		
		contactPhysics->kn = contactPhysics->initialKn;
		contactPhysics->ks = contactPhysics->initialKs;
		contactPhysics->equilibriumDistance = contactPhysics->initialEquilibriumDistance;
*/
		if(!interaction->interactionPhysics)
		{
			interaction->interactionPhysics = shared_ptr<ElasticContactInteraction>(new ElasticContactInteraction());
			ElasticContactInteraction* contactPhysics = YADE_CAST<ElasticContactInteraction*>(interaction->interactionPhysics.get());

			Real Ea 	= sdec1->young;
			Real Eb 	= sdec2->young;
			Real Va 	= sdec1->poisson;
			Real Vb 	= sdec2->poisson;
			Real Da 	= interactionGeometry->radius1; // FIXME - multiply by factor of sphere interaction distance (so sphere intaracts at bigger range that its geometrical size)
			Real Db 	= interactionGeometry->radius2; // FIXME - as above
			Real fa 	= sdec1->frictionAngle;
			Real fb 	= sdec2->frictionAngle;

			Real Eab	= 2*Ea*Eb/(Ea+Eb);
			Real Vab	= 2*Va*Vb/(Va+Vb);

			Real Dinit 	= Da+Db; 			// FIXME - is it just a sum?
			Real Sinit 	= Mathr::PI * std::pow( std::min(Da,Db) , 2);

			Real Kn						= (Eab*Sinit/Dinit)*( (1+alpha)/(beta*(1+Vab) + gamma*(1-alpha*Vab) ) );
//cerr << "Kn: " << Kn << endl;
			contactPhysics->initialKn			= Kn;
			contactPhysics->initialKs			= Kn*(1-alpha*Vab)/(1+Vab);
//cerr << "Ks: " <<       contactPhysics->initialKs			<< endl;
			contactPhysics->frictionAngle			= (fa+fb)*0.5; // FIXME - this is actually a waste of memory space, just like initialKs and initialKn
			contactPhysics->tangensOfFrictionAngle		= std::tan(contactPhysics->frictionAngle); 

			contactPhysics->prevNormal 			= interactionGeometry->normal;
			contactPhysics->initialEquilibriumDistance	= Dinit;			

			contactPhysics->kn = contactPhysics->initialKn;
			contactPhysics->ks = contactPhysics->initialKs;
			contactPhysics->equilibriumDistance = contactPhysics->initialEquilibriumDistance;

		}
		else
		{	// FIXME - are those lines necessary ???? what they are doing in fact ???
			ElasticContactInteraction* contactPhysics = YADE_CAST<ElasticContactInteraction*>(interaction->interactionPhysics.get());
			if(contactPhysics)
			{
				contactPhysics->kn = contactPhysics->initialKn;
				contactPhysics->ks = contactPhysics->initialKs;
				contactPhysics->equilibriumDistance = contactPhysics->initialEquilibriumDistance;
			}
		}	
		
	}
	else   // this is PERMANENT LINK because previous dynamic_cast failed, dispatcher should do this job
	{
#if 0
		SDECLinkGeometry* sdecLinkGeometry =  dynamic_cast<SDECLinkGeometry*>(interaction->interactionGeometry.get());
		if (sdecLinkGeometry)
		{		
			SDECLinkPhysics* linkPhysics = static_cast<SDECLinkPhysics*>(interaction->interactionPhysics.get());
	//		linkPhysics->frictionAngle 		= ?? //FIXME - uninitialized 
			linkPhysics->kn 			= linkPhysics->initialKn;
			linkPhysics->ks 			= linkPhysics->initialKs;
			linkPhysics->equilibriumDistance 	= linkPhysics->initialEquilibriumDistance;
		}
		else
#endif			
		if(dynamic_cast<TetraBang*>(interaction->interactionGeometry.get()) && !interaction->interactionPhysics){
			// TetraLaw doesn't use InteractionPhysics anyway, it accesses bodies' parameters directly
			interaction->interactionPhysics=shared_ptr<InteractionPhysics>(new InteractionPhysics);
		}
	}
};
YADE_PLUGIN((MacroMicroElasticRelationships));

YADE_REQUIRE_FEATURE(PHYSPAR);

