/*************************************************************************
*  Copyright (C) 2007 by Bruno CHAREYRE                                 *
*  bruno.chareyre@hmg.inpg.fr                                        *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"Ip2_FrictMat_FrictMat_FrictPhys.hpp"
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/pkg-dem/DemXDofGeom.hpp>
#include<yade/pkg-dem/FrictPhys.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>
#include<yade/pkg-common/ElastMat.hpp>



void Ip2_FrictMat_FrictMat_FrictPhys::go(	  const shared_ptr<Material>& b1
					, const shared_ptr<Material>& b2
					, const shared_ptr<Interaction>& interaction)
{
	
	//ScGeom* interactionGeometry = YADE_CAST<ScGeom*>(interaction->interactionGeometry.get());
	
	//if(interactionGeometry)
	{
		if(!interaction->interactionPhysics)
		{
			const shared_ptr<FrictMat>& mat1 = YADE_PTR_CAST<FrictMat>(b1);
			const shared_ptr<FrictMat>& mat2 = YADE_PTR_CAST<FrictMat>(b2);
			
			if (!interaction->interactionPhysics) interaction->interactionPhysics = shared_ptr<FrictPhys>(new FrictPhys());
			
			const shared_ptr<FrictPhys>& contactPhysics = YADE_PTR_CAST<FrictPhys>(interaction->interactionPhysics);

			Real Ea 	= mat1->young;
			Real Eb 	= mat2->young;
			Real Va 	= mat1->poisson;
			Real Vb 	= mat2->poisson;
			#if 0
				Real Da 	= interactionGeometry->radius1; // FIXME - multiply by factor of sphere interaction distance (so sphere interacts at bigger range that its geometrical size)
				Real Db 	= interactionGeometry->radius2; // FIXME - as above
				Vector3r normal=interactionGeometry->normal;
			#else
				Real Da,Db; Vector3r normal;
				ScGeom* scg=dynamic_cast<ScGeom*>(interaction->interactionGeometry.get());
				Dem3DofGeom* d3dg=dynamic_cast<Dem3DofGeom*>(interaction->interactionGeometry.get());
				if(scg){ Da=scg->radius1; Db=scg->radius2; normal=scg->normal; }
				else if(d3dg){Da=d3dg->refR1>0?d3dg->refR1:2*d3dg->refR2; Db=d3dg->refR2>0?d3dg->refR2:d3dg->refR1; normal=d3dg->normal; }
				else throw runtime_error("Ip2_FrictMat_FrictMat_FrictPhys: geometry is neither ScGeom nor Dem3DofGeom");
			#endif
			
			Real fa 	= mat1->frictionAngle;
			Real fb 	= mat2->frictionAngle;

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

			contactPhysics->prevNormal 			= normal;
			contactPhysics->initialEquilibriumDistance	= Dinit;			

			contactPhysics->kn = contactPhysics->initialKn;
			contactPhysics->ks = contactPhysics->initialKs;
			contactPhysics->equilibriumDistance = contactPhysics->initialEquilibriumDistance;

		}	
		return;
	}
	throw runtime_error("Ip2_FrictMat_FrictMat_FrictPhys currently fails for non-ScGeom geometry!");
};
YADE_PLUGIN((Ip2_FrictMat_FrictMat_FrictPhys));

