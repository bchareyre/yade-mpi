/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*  Copyright (C) 2008 by Janek Kozicki <cosurgi@berlios.de>              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/GlobalEngine.hpp>
#include<yade/pkg-common/LawFunctor.hpp>
#include <set>
#include <boost/tuple/tuple.hpp>

class Law2_ScGeom_CohFrictPhys_ElasticPlastic: public LawFunctor{
	public:
	virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, Scene*);
	YADE_CLASS_BASE_DOC_ATTRS(Law2_ScGeom_CohFrictPhys_ElasticPlastic,LawFunctor,"Law for linear traction-compression-bending-twisting, with cohesion+friction and Mohr-Coulomb plasticity surface. Can be elastic-fragile or perfectly elastic-plastic. Creep at contact can be enabled.\n\n.. note::\n This law uses :yref:`ScGeom`.",
		((bool,neverErase,false,"Keep interactions even if particles go away from each other (only in case another constitutive law is in the scene, e.g. :yref:`Law2_ScGeom_CapillaryPhys_Capillarity`)"))
		((bool,momentRotationLaw,false,"use bending/twisting moment at contacts. See :yref:`CohesiveFrictionalContactLaw::always_use_moment_law` for details."))
		((bool,always_use_moment_law,false,"If true, use bending/twisting moments at all contacts. If false, compute moments only for cohesive contacts."))
		((bool,shear_creep,false,"activate creep on the shear force, using :yref:`CohesiveFrictionalContactLaw::creep_viscosity`."))
		((bool,twist_creep,false,"activate creep on the twisting moment, using :yref:`CohesiveFrictionalContactLaw::creep_viscosity`."))
		((Real,creep_viscosity,1,"creep viscosity [Pa.s/m]. probably should be moved to Ip2_2xCohFrictMat_CohFrictPhys..."))
		((bool,erosionActivated,false,""))
		((bool,detectBrokenBodies,false,""))
		
	);
	FUNCTOR2D(ScGeom,CohFrictPhys);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom_CohFrictPhys_ElasticPlastic);


class CohesiveFrictionalContactLaw : public GlobalEngine
{
	shared_ptr<Law2_ScGeom_CohFrictPhys_ElasticPlastic> functor;
	
	public :		
		long iter;/// used for checking if new iteration
		void action();
		
	YADE_CLASS_BASE_DOC_ATTRS(CohesiveFrictionalContactLaw,GlobalEngine,"[DEPRECATED] Loop over interactions applying :yref:`Law2_ScGeom_CohFrictPhys_ElasticPlastic` on all interactions.\n\n.. note::\n  Use :yref:`InteractionDispatchers` and :yref:`Law2_ScGeom_CohFrictPhys_ElasticPlastic` instead of this class for performance reasons.",
		((bool,neverErase,false,"Keep interactions even if particles go away from each other (only in case another constitutive law is in the scene, e.g. :yref:`Law2_ScGeom_CapillaryPhys_Capillarity`)"))
		((bool,momentRotationLaw,false,"use bending/twisting moment at contacts. See :yref:`CohesiveFrictionalContactLaw::always_use_moment_law` for details."))
		((bool,always_use_moment_law,false,"If true, use bending/twisting moments at all contacts. If false, compute moments only for cohesive contacts."))
		((bool,shear_creep,false,"activate creep on the shear force, using :yref:`CohesiveFrictionalContactLaw::creep_viscosity`."))
		((bool,twist_creep,false,"activate creep on the twisting moment, using :yref:`CohesiveFrictionalContactLaw::creep_viscosity`."))
		((Real,creep_viscosity,false,"creep viscosity [Pa.s/m]. probably should be moved to Ip2_2xCohFrictMat_CohFrictPhys..."))
		((bool,erosionActivated,false,""))
		((bool,detectBrokenBodies,false,""))
		
	);
};

REGISTER_SERIALIZABLE(CohesiveFrictionalContactLaw);

