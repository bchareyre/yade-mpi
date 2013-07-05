/*************************************************************************
*  Copyright (C) 2012 by Ignacio Olmedo nolmedo.manich@gmail.com         *
*  Copyright (C) 2012 by François Kneib   francois.kneib@gmail.com       *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#pragma once
#include "Ip2_CohFrictMat_CohFrictMat_CohFrictPhys.hpp"
#include "InelastCohFrictMat.hpp"
#include "InelastCohFrictPhys.hpp"
#include<yade/pkg/dem/ScGeom.hpp>


class Ip2_2xInelastCohFrictMat_InelastCohFrictPhys : public IPhysFunctor
{
	public :
		virtual void go(	const shared_ptr<Material>& b1,
					const shared_ptr<Material>& b2,
					const shared_ptr<Interaction>& interaction);
		int cohesionDefinitionIteration;

		YADE_CLASS_BASE_DOC_ATTRS_CTOR(Ip2_2xInelastCohFrictMat_InelastCohFrictPhys,IPhysFunctor,
		"Generates cohesive-frictional interactions with moments. Used in the contact law :yref:`Law2_ScGeom6D_InelastCohFrictPhys_CohesionMoment`.",
		,
		cohesionDefinitionIteration = -1;
		);
	FUNCTOR2D(InelastCohFrictMat,InelastCohFrictMat);
};

REGISTER_SERIALIZABLE(Ip2_2xInelastCohFrictMat_InelastCohFrictPhys);