/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/GlobalEngine.hpp>

// only to see whether SCG_SHEAR is defined, may be removed in the future
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/pkg-common/LawFunctor.hpp>

#include <set>
#include <boost/tuple/tuple.hpp>


class Law2_ScGeom_FrictPhys_Basic: public LawFunctor{
	public:
		virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, Scene*);
	YADE_CLASS_BASE_DOC_ATTRS(Law2_ScGeom_FrictPhys_Basic,LawFunctor,"Law for linear compression, without cohesion and Mohr-Coulomb plasticity surface.\n\n.. note::\n This law uses :yref:`ScGeom`; there is also functionally equivalent :yref:`Law2_Dem3DofGeom_FrictPhys_Basic`, which uses :yref:`Dem3DofGeom`.",
		((int,sdecGroupMask,1,"Bitmask for allowing collision between particles :yref:`Body::groupMask`"))
		((bool,neverErase,false,"Keep interactions even if particles go away from each other [for debugging?]"))
		#ifdef SCG_SHEAR
			((bool,useShear,false,"Use ScGeom::updateShear rather than ScGeom::updateShearForce for shear force computation."))
		#endif
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

/* Class for demonstrating beam-like behavior of the contact (normal, shear, bend and twist) */
class Law2_Dem6DofGeom_FrictPhys_Beam: public LawFunctor{
	public:
		virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, Scene*);
	FUNCTOR2D(Dem6DofGeom,FrictPhys);
	YADE_CLASS_BASE_DOC(Law2_Dem6DofGeom_FrictPhys_Beam,LawFunctor,"Class for demonstrating beam-like behavior of contact (normal, shear, bend and twist) [broken][experimental]");
};
REGISTER_SERIALIZABLE(Law2_Dem6DofGeom_FrictPhys_Beam);

class ElasticContactLaw : public GlobalEngine{
		shared_ptr<Law2_ScGeom_FrictPhys_Basic> functor;
	public :
		void action(Scene*);
	YADE_CLASS_BASE_DOC_ATTRS(ElasticContactLaw,GlobalEngine,"[DEPRECATED] Loop over interactions applying :yref:`Law2_ScGeom_FrictPhys_Basic` on all interactions.\n\n.. note::\n  Use :yref:`InteractionDispatchers` and :yref:`Law2_ScGeom_FrictPhys_Basic` instead of this class for performance reasons.",
		((int,sdecGroupMask,1,"Bitmask for allowing collision between particles :yref:`Body::groupMask`"))
		((bool,neverErase,false,"Keep interactions even if particles go away from each other [for debugging?]"))
		#ifdef SCG_SHEAR
			((bool,useShear,false,"Use :yref:`ScGeom`::updateShear rather than :yref:`ScGeom`::updateShearForce for shear force computation."))
		#endif
	);
};

REGISTER_SERIALIZABLE(ElasticContactLaw);



