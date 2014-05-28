/*************************************************************************
*  Copyright (C) 2012 by Ignacio Olmedo nolmedo.manich@gmail.com         *
*  Copyright (C) 2012 by François Kneib   francois.kneib@gmail.com       *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#pragma once
#include "CohesiveFrictionalContactLaw.hpp"
#include "InelastCohFrictPhys.hpp"

class Law2_ScGeom6D_InelastCohFrictPhys_CohesionMoment: public LawFunctor{
	public:
		Real normElastEnergy();
		Real shearElastEnergy();
	virtual void go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I);
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Law2_ScGeom6D_InelastCohFrictPhys_CohesionMoment,LawFunctor,"This law is currently under developpement. Final version and documentation will come before the end of 2014.",
		,,
		.def("normElastEnergy",&Law2_ScGeom6D_InelastCohFrictPhys_CohesionMoment::normElastEnergy,"Compute normal elastic energy.")
		.def("shearElastEnergy",&Law2_ScGeom6D_InelastCohFrictPhys_CohesionMoment::shearElastEnergy,"Compute shear elastic energy.")
	);
	FUNCTOR2D(ScGeom6D,InelastCohFrictPhys);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom6D_InelastCohFrictPhys_CohesionMoment);
