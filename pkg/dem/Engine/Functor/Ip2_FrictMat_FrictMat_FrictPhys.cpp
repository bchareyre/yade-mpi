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



void Ip2_FrictMat_FrictMat_FrictPhys::go( const shared_ptr<Material>& b1
					, const shared_ptr<Material>& b2
					, const shared_ptr<Interaction>& interaction)
{
	if(interaction->interactionPhysics) return;

		const shared_ptr<FrictMat>& mat1 = YADE_PTR_CAST<FrictMat>(b1);
		const shared_ptr<FrictMat>& mat2 = YADE_PTR_CAST<FrictMat>(b2);
		if (!interaction->interactionPhysics)
			interaction->interactionPhysics = shared_ptr<FrictPhys>(new FrictPhys());
		const shared_ptr<FrictPhys>& contactPhysics = YADE_PTR_CAST<FrictPhys>(interaction->interactionPhysics);

		Real Ea 	= mat1->young;
		Real Eb 	= mat2->young;
		Real Va 	= mat1->poisson;
		Real Vb 	= mat2->poisson;
		
		Real Da,Db; Vector3r normal;
 		//FIXME : dynamic casts here???!!!
		ScGeom* scg=dynamic_cast<ScGeom*>(interaction->interactionGeometry.get());
		Dem3DofGeom* d3dg=dynamic_cast<Dem3DofGeom*>(interaction->interactionGeometry.get());
		if(scg){Da=scg->radius1; Db=scg->radius2; normal=scg->normal;}
		else if(d3dg){Da=d3dg->refR1>0?d3dg->refR1:2*d3dg->refR2; Db=d3dg->refR2>0?d3dg->refR2:d3dg->refR1; normal=d3dg->normal;}
		else throw runtime_error("Ip2_FrictMat_FrictMat_FrictPhys: geometry is neither ScGeom nor Dem3DofGeom");
 		//harmonic average of the two stiffnesses when (Di.Ei/2) is the stiffness of sphere "i"
		Real Kn = 2*Ea*Da*Eb*Db/(Ea*Da+Eb*Db);
		//same for shear stiffness
		Real Ks = 2*Ea*Da*Va*Eb*Db*Vb/(Ea*Da*Va+Eb*Db*Va);
		
		contactPhysics->frictionAngle = std::min(mat1->frictionAngle,mat2->frictionAngle);
		contactPhysics->tangensOfFrictionAngle = std::tan(contactPhysics->frictionAngle); 
		contactPhysics->prevNormal = normal;
		contactPhysics->kn = Kn;
		contactPhysics->ks = Ks;
		return;
};
YADE_PLUGIN((Ip2_FrictMat_FrictMat_FrictPhys));

