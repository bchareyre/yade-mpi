/*************************************************************************
*  Copyright (C) 2005 by Bruno Chareyre   bruno.chareyre@hmg.inpg.fr     *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/GlobalEngine.hpp>
#include<yade/pkg/common/Dispatching.hpp>

#include <set>
#include <boost/tuple/tuple.hpp>
#include<yade/lib/base/openmp-accu.hpp>

class Law2_ScGeom_FrictPhys_CundallStrack: public LawFunctor{
	public:
		//OpenMPAccumulator<Real> plasticDissipation;
		virtual void go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I);
		//Real elasticEnergy ();
		//Real getPlasticDissipation();
		//void initPlasticDissipation(Real initVal=0);
		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Law2_ScGeom_FrictPhys_CundallStrack,LawFunctor,"Law for linear compression, and Mohr-Coulomb plasticity surface without cohesion.\nThis law implements the classical linear elastic-plastic law from [CundallStrack1979]_ (see also [Pfc3dManual30]_). The normal force is (with the convention of positive tensile forces) $F_n=\\min(k_n u_n, 0)$. The shear force is $F_s=k_s u_s$, the plasticity condition defines the maximum value of the shear force : $F_s^{\\max}=F_n\\tan(\\phi)$, with $\\phi$ the friction angle.\n\n.. note::\n This law uses :yref:`ScGeom`; there is also functionally equivalent :yref:`Law2_Dem3DofGeom_FrictPhys_CundallStrack`, which uses :yref:`Dem3DofGeom` (sphere-box interactions are not implemented for the latest).\n\n.. note::\n This law is well tested in the context of triaxial simulation, and has been used for a number of published results (see e.g. [Scholtes2009b]_ and other papers from the same authors). It is generalised by :yref:`Law2_ScGeom_CohFrictPhys_CohesionMoment`, which adds cohesion and moments at contact.",
		((bool,neverErase,false,,"Keep interactions even if particles go away from each other (only in case another constitutive law is in the scene, e.g. :yref:`Law2_ScGeom_CapillaryPhys_Capillarity`)"))
		((bool,traceEnergy,false,Attr::hidden,"Define the total energy dissipated in plastic slips at all contacts."))
		((int,plastDissipIx,-1,(Attr::hidden|Attr::noSave),"Index for plastic dissipation (with O.trackEnergy)"))
		((int,elastPotentialIx,-1,(Attr::hidden|Attr::noSave),"Index for elastic potential energy (with O.trackEnergy)"))
		,,
		//.def("elasticEnergy",&Law2_ScGeom_FrictPhys_CundallStrack::elasticEnergy,"Compute and return the total elastic energy in all \"FrictPhys\" contacts")
		//.def("plasticDissipation",&Law2_ScGeom_FrictPhys_CundallStrack::getPlasticDissipation,"Total energy dissipated in plastic slips at all FrictPhys contacts. Computed only if :yref:`Law2_ScGeom_FrictPhys_CundallStrack::traceEnergy` is true.")
		//.def("initPlasticDissipation",&Law2_ScGeom_FrictPhys_CundallStrack::initPlasticDissipation,"Initialize cummulated plastic dissipation to a value (0 by default).")
	);
	FUNCTOR2D(ScGeom,FrictPhys);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom_FrictPhys_CundallStrack);

/* Constitutive law for linear compression, no tension, and linear plasticity surface.

This class serves also as tutorial and is documented in detail at

	https://yade-dem.org/index.php/ConstitutiveLawHowto
*/
class Law2_Dem3DofGeom_FrictPhys_CundallStrack: public LawFunctor{
	public:
		virtual void go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I);
		FUNCTOR2D(Dem3DofGeom,FrictPhys);
		YADE_CLASS_BASE_DOC(Law2_Dem3DofGeom_FrictPhys_CundallStrack,LawFunctor,"Constitutive law for linear compression, no tension, and linear plasticity surface.\n\nThis class serves also as tutorial and is documented in detail at https://yade-dem.org/index.php/ConstitutiveLawHowto.");
};
REGISTER_SERIALIZABLE(Law2_Dem3DofGeom_FrictPhys_CundallStrack);

class ElasticContactLaw : public GlobalEngine{
		shared_ptr<Law2_ScGeom_FrictPhys_CundallStrack> functor;
	public :
		void action();
	YADE_CLASS_BASE_DOC_ATTRS(ElasticContactLaw,GlobalEngine,"[DEPRECATED] Loop over interactions applying :yref:`Law2_ScGeom_FrictPhys_CundallStrack` on all interactions.\n\n.. note::\n  Use :yref:`InteractionLoop` and :yref:`Law2_ScGeom_FrictPhys_CundallStrack` instead of this class for performance reasons.",
		((bool,neverErase,false,,"Keep interactions even if particles go away from each other (only in case another constitutive law is in the scene, e.g. :yref:`Law2_ScGeom_CapillaryPhys_Capillarity`)"))
	);
};

REGISTER_SERIALIZABLE(ElasticContactLaw);



