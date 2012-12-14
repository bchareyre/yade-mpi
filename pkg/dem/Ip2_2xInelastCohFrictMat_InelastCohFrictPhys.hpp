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
		((bool,setCohesionNow,false,,"If true, assign cohesion to all existing contacts in current time-step. The flag is turned false automatically, so that assignment is done in the current timestep only."))
		((bool,setCohesionOnNewContacts,false,,"If true, assign cohesion at all new contacts. If false, only existing contacts can be cohesive (also see :yref:`Ip2_CohFrictMat_CohFrictMat_CohFrictPhys::setCohesionNow`), and new contacts are only frictional."))	
		,
		cohesionDefinitionIteration = -1;
		);
	FUNCTOR2D(InelastCohFrictMat,InelastCohFrictMat);
};

REGISTER_SERIALIZABLE(Ip2_2xInelastCohFrictMat_InelastCohFrictPhys);