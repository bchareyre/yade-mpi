/*************************************************************************
*  Copyright (C) 2007 by Bruno CHAREYRE                                 *
*  bruno.chareyre@hmg.inpg.fr                                        *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"Ip2_Frictmat_FrictMat_CapillaryLawPhys.hpp"
#include<yade/pkg-dem/ScGeom.hpp>
#include <yade/pkg-dem/CapillaryParameters.hpp>
#include<yade/pkg-dem/FrictPhys.hpp>
#include<yade/pkg-common/ElastMat.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>

YADE_PLUGIN((Ip2_Frictmat_FrictMat_CapillaryLawPhys));

void Ip2_Frictmat_FrictMat_CapillaryLawPhys::go( const shared_ptr<Material>& b1 //FrictMat
					, const shared_ptr<Material>& b2 // FrictMat
					, const shared_ptr<Interaction>& interaction)
{
	
	ScGeom* interactionGeometry = YADE_CAST<ScGeom*>(interaction->interactionGeometry.get());
	
	if(interactionGeometry) // so it is ScGeom  - NON PERMANENT LINK
	{
//cerr << "interactionGeometry" << endl;
		if(!interaction->interactionPhysics)
		{
//cerr << "interaction->isNew" << endl;
			
// 			Body* de1 = (*bodies)[id1].get();
// 			Body* de2 = (*bodies)[id2].get();
 			const shared_ptr<FrictMat>& sdec1 = YADE_PTR_CAST<FrictMat>(b1);
 			const shared_ptr<FrictMat>& sdec2 = YADE_PTR_CAST<FrictMat>(b2);
			
 			if (!interaction->interactionPhysics) interaction->interactionPhysics = shared_ptr<CapillaryParameters>(new CapillaryParameters());
//			interaction->interactionPhysics = shared_ptr<CapillaryParameters>(new CapillaryParameters());
			const shared_ptr<CapillaryParameters>& contactPhysics = YADE_PTR_CAST<CapillaryParameters>(interaction->interactionPhysics);

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


	//This is the formula used in PFC-3D
	//
	//Real Kn = 4 * ((Ea+Eb)*0.5) * ((Da+Db)*0.5);
	//Real Ks = Kn/2.0;


			contactPhysics->initialKn			= Kn;
			contactPhysics->initialKs			= Ks;
//cerr << "Ks: " <<       contactPhysics->initialKs			<< endl;
			contactPhysics->frictionAngle			= std::min(fa,fb); // FIXME - this is actually a waste of memory space, just like initialKs and initialKn
//cerr << "contactPhysics->frictionAngle " << contactPhysics->frictionAngle << " "<< fa << " " << fb << endl;
			contactPhysics->tangensOfFrictionAngle		= std::tan(contactPhysics->frictionAngle); 
//cerr << "contactPhysics->tangensOfFrictionAngle " << contactPhysics->tangensOfFrictionAngle << endl;

			contactPhysics->prevNormal 			= interactionGeometry->normal;
			contactPhysics->initialEquilibriumDistance	= Dinit;			

			contactPhysics->kn = contactPhysics->initialKn;
			contactPhysics->ks = contactPhysics->initialKs;
			contactPhysics->equilibriumDistance = contactPhysics->initialEquilibriumDistance;

		}
		
		
	}

};




