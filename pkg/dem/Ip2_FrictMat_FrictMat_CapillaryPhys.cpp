/*************************************************************************
*  Copyright (C) 2007 by Bruno CHAREYRE                                 *
*  bruno.chareyre@hmg.inpg.fr                                        *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"Ip2_FrictMat_FrictMat_CapillaryPhys.hpp"
#include<yade/pkg/dem/ScGeom.hpp>
#include <yade/pkg-dem/CapillaryPhys.hpp>
#include<yade/pkg/dem/FrictPhys.hpp>
#include<yade/pkg/common/ElastMat.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>

YADE_PLUGIN((Ip2_FrictMat_FrictMat_CapillaryPhys));

void Ip2_FrictMat_FrictMat_CapillaryPhys::go( const shared_ptr<Material>& b1 //FrictMat
					, const shared_ptr<Material>& b2 // FrictMat
					, const shared_ptr<Interaction>& interaction)
{	
	ScGeom* geom = YADE_CAST<ScGeom*>(interaction->geom.get());
	if(geom) 
	{
		if(!interaction->phys)
		{
 			const shared_ptr<FrictMat>& sdec1 = YADE_PTR_CAST<FrictMat>(b1);
 			const shared_ptr<FrictMat>& sdec2 = YADE_PTR_CAST<FrictMat>(b2);
			
 			if (!interaction->phys) interaction->phys = shared_ptr<CapillaryPhys>(new CapillaryPhys());
			const shared_ptr<CapillaryPhys>& contactPhysics = YADE_PTR_CAST<CapillaryPhys>(interaction->phys);

			Real Ea 	= sdec1->young;
			Real Eb 	= sdec2->young;
			Real Va 	= sdec1->poisson;
			Real Vb 	= sdec2->poisson;
			Real Da 	= geom->radius1; // FIXME - multiply by factor of sphere interaction distance (so sphere interacts at bigger range that its geometrical size)
			Real Db 	= geom->radius2; // FIXME - as above
			Real fa 	= sdec1->frictionAngle;
			Real fb 	= sdec2->frictionAngle;
			Real Kn = 2*Ea*Da*Eb*Db/(Ea*Da+Eb*Db);//harmonic average of two stiffnesses
			Real Ks = 2*Ea*Da*Va*Eb*Db*Vb/(Ea*Da*Va+Eb*Db*Va);//harmonic average of two stiffnesses with ks=V*kn for each sphere
			contactPhysics->frictionAngle			= std::min(fa,fb);
			contactPhysics->tangensOfFrictionAngle		= std::tan(contactPhysics->frictionAngle); 
			contactPhysics->prevNormal 			= geom->normal;
			contactPhysics->kn = Kn;
			contactPhysics->ks = Ks;
		}
	}
};




