/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*  Copyright (C) 2008 by Janek Kozicki <cosurgi@berlios.de>              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg/common/Dispatching.hpp>

class Ip2_2xCohFrictMat_CohFrictPhys : public IPhysFunctor
{
	public :
		virtual void go(	const shared_ptr<Material>& b1,
					const shared_ptr<Material>& b2,
					const shared_ptr<Interaction>& interaction);
		int cohesionDefinitionIteration;

		YADE_CLASS_BASE_DOC_ATTRS_CTOR(Ip2_2xCohFrictMat_CohFrictPhys,IPhysFunctor,
		"Generates cohesive-frictional interactions with moments. Used in the contact law :yref:`Law2_ScGeom6D_CohFrictPhys_CohesionMoment`.",
		((bool,setCohesionNow,false,,"If true, assign cohesion to all existing contacts in current time-step. The flag is turned false automatically, so that assignment is done in the current timestep only."))
		((bool,setCohesionOnNewContacts,false,,"If true, assign cohesion at all new contacts. If false, only existing contacts can be cohesive (also see :yref:`Ip2_2xCohFrictMat_CohFrictPhys::setCohesionNow`), and new contacts are only frictional."))	
		,
		cohesionDefinitionIteration = -1;
		);
	FUNCTOR2D(CohFrictMat,CohFrictMat);
};

REGISTER_SERIALIZABLE(Ip2_2xCohFrictMat_CohFrictPhys);


