/*************************************************************************
*  Copyright (C) 2005 by Bruno Chareyre   bruno.chareyre@hmg.inpg.fr     *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/GlobalEngine.hpp>
#include<yade/pkg-common/LawFunctor.hpp>

#include <set>
#include <boost/tuple/tuple.hpp>
#include<yade/lib-base/openmp-accu.hpp>

class Law2_ScGeom_FrictPhys_Basic: public LawFunctor{
	public:
		static Real Real0;
		OpenMPAccumulator<Real,&Law2_ScGeom_FrictPhys_Basic::Real0> plasticDissipation;
		virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, Scene*);
		Real elasticEnergy ();
		Real getPlasticDissipation();
		void initPlasticDissipation(Real initVal=0);
		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Law2_ScGeom_FrictPhys_Basic,LawFunctor,"Law for linear compression, without cohesion and Mohr-Coulomb plasticity surface.\n\n.. note::\n This law uses :yref:`ScGeom`; there is also functionally equivalent :yref:`Law2_Dem3DofGeom_FrictPhys_Basic`, which uses :yref:`Dem3DofGeom` (sphere-box interactions are not implemented for the latest).",
		((bool,neverErase,false,"Keep interactions even if particles go away from each other (only in case another constitutive law is in the scene, e.g. :yref:`Law2_ScGeom_CapillaryPhys_Capillarity`)"))
		((bool,useShear,false,"Use ScGeom::updateShear rather than ScGeom::rotateAndGetShear for shear force computation."))
		((bool,traceEnergy,false,"Define the total energy dissipated in plastic slips at all contacts."))
		,,
		.def("elasticEnergy",&Law2_ScGeom_FrictPhys_Basic::elasticEnergy,"Compute and return the total elastic energy in all \"FrictPhys\" contacts")
		.def("plasticDissipation",&Law2_ScGeom_FrictPhys_Basic::getPlasticDissipation,"Total energy dissipated in plastic slips at all FrictPhys contacts. Computed only if :yref:`Law2_ScGeom_FrictPhys_Basic::traceEnergy` is true.")
		.def("initPlasticDissipation",&Law2_ScGeom_FrictPhys_Basic::initPlasticDissipation,"Initialize cummulated plastic dissipation to a value (0 by default).")
	);
	FUNCTOR2D(ScGeom,FrictPhys);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom_FrictPhys_Basic);

/* Constitutive law for linear compression, no tension, and linear plasticity surface.

This class serves also as tutorial and is documented in detail at

	https://yade-dem.org/index.php/ConstitutiveLawHowto
*/
class Law2_Dem3DofGeom_FrictPhys_Basic: public LawFunctor{
	public:
		virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, Scene*);
		FUNCTOR2D(Dem3DofGeom,FrictPhys);
		YADE_CLASS_BASE_DOC(Law2_Dem3DofGeom_FrictPhys_Basic,LawFunctor,"Constitutive law for linear compression, no tension, and linear plasticity surface.\n\nThis class serves also as tutorial and is documented in detail at https://yade-dem.org/index.php/ConstitutiveLawHowto.");
};
REGISTER_SERIALIZABLE(Law2_Dem3DofGeom_FrictPhys_Basic);

class ElasticContactLaw : public GlobalEngine{
		shared_ptr<Law2_ScGeom_FrictPhys_Basic> functor;
	public :
		void action();
	YADE_CLASS_BASE_DOC_ATTRS(ElasticContactLaw,GlobalEngine,"[DEPRECATED] Loop over interactions applying :yref:`Law2_ScGeom_FrictPhys_Basic` on all interactions.\n\n.. note::\n  Use :yref:`InteractionDispatchers` and :yref:`Law2_ScGeom_FrictPhys_Basic` instead of this class for performance reasons.",
		((bool,neverErase,false,"Keep interactions even if particles go away from each other (only in case another constitutive law is in the scene, e.g. :yref:`Law2_ScGeom_CapillaryPhys_Capillarity`)"))
		((bool,useShear,false,"Use :yref:`ScGeom`::updateShear rather than :yref:`ScGeom`::rotateAndGetShear for shear force computation."))
	);
};

REGISTER_SERIALIZABLE(ElasticContactLaw);



