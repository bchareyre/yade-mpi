/*************************************************************************
*  Copyright (C) 2007 by Bruno CHAREYRE                                 *
*  bruno.chareyre@hmg.inpg.fr                                        *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"SimpleElasticRelationships.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/DemXDofGeom.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>



void SimpleElasticRelationships::go(	  const shared_ptr<PhysicalParameters>& b1 // BodyMacroParameters
					, const shared_ptr<PhysicalParameters>& b2 // BodyMacroParameters
					, const shared_ptr<Interaction>& interaction)
{
	
	//SpheresContactGeometry* interactionGeometry = YADE_CAST<SpheresContactGeometry*>(interaction->interactionGeometry.get());
	
	//if(interactionGeometry)
	{
		if(!interaction->interactionPhysics)
		{
			const shared_ptr<BodyMacroParameters>& sdec1 = YADE_PTR_CAST<BodyMacroParameters>(b1);
			const shared_ptr<BodyMacroParameters>& sdec2 = YADE_PTR_CAST<BodyMacroParameters>(b2);
			
			if (!interaction->interactionPhysics) interaction->interactionPhysics = shared_ptr<ElasticContactInteraction>(new ElasticContactInteraction());
			
			const shared_ptr<ElasticContactInteraction>& contactPhysics = YADE_PTR_CAST<ElasticContactInteraction>(interaction->interactionPhysics);

			Real Ea 	= sdec1->young;
			Real Eb 	= sdec2->young;
			Real Va 	= sdec1->poisson;
			Real Vb 	= sdec2->poisson;
			#if 0
				Real Da 	= interactionGeometry->radius1; // FIXME - multiply by factor of sphere interaction distance (so sphere interacts at bigger range that its geometrical size)
				Real Db 	= interactionGeometry->radius2; // FIXME - as above
				Vector3r normal=interactionGeometry->normal;
			#else
				Real Da,Db; Vector3r normal;
				SpheresContactGeometry* scg=dynamic_cast<SpheresContactGeometry*>(interaction->interactionGeometry.get());
				Dem3DofGeom* d3dg=dynamic_cast<Dem3DofGeom*>(interaction->interactionGeometry.get());
				if(scg){ Da=scg->radius1; Db=scg->radius2; normal=scg->normal; }
				else if(d3dg){Da=d3dg->refR1>0?d3dg->refR1:2*d3dg->refR2; Db=d3dg->refR2>0?d3dg->refR2:d3dg->refR1; normal=d3dg->normal; }
				else throw runtime_error("SimpleElasticRelationships: geometry is neither SpheresContactGeometry nor Dem3DofGeom");
			#endif
			
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

			contactPhysics->prevNormal 			= normal;
			contactPhysics->initialEquilibriumDistance	= Dinit;			

			contactPhysics->kn = contactPhysics->initialKn;
			contactPhysics->ks = contactPhysics->initialKs;
			contactPhysics->equilibriumDistance = contactPhysics->initialEquilibriumDistance;

		}	
		return;
	}
	throw runtime_error("SimpleElasticRelationships currently fails for non-SpheresContactGeometry geometry!");
};
YADE_PLUGIN("SimpleElasticRelationships");