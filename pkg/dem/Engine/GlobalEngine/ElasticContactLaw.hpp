/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/InteractionSolver.hpp>

// only to see whether SCG_SHEAR is defined, may be removed in the future
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/pkg-common/LawFunctor.hpp>

#include <set>
#include <boost/tuple/tuple.hpp>


class Law2_ScGeom_FrictPhys_Basic: public LawFunctor{
	public:
	virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, Scene* rootBody);
	int sdecGroupMask;
	bool momentRotationLaw;	
	bool neverErase;	

	#ifdef SCG_SHEAR
		bool useShear;
	#endif
	
	Law2_ScGeom_FrictPhys_Basic(): sdecGroupMask(1), momentRotationLaw(true), neverErase(false)
		#ifdef SCG_SHEAR
			, useShear(false)
		#endif
		{}
	FUNCTOR2D(ScGeom,FrictPhys);
	REGISTER_CLASS_AND_BASE(Law2_ScGeom_FrictPhys_Basic,LawFunctor);
	REGISTER_ATTRIBUTES(LawFunctor,(sdecGroupMask)(momentRotationLaw)(neverErase)
		#ifdef SCG_SHEAR
			(useShear)
		#endif
	);
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
		REGISTER_CLASS_AND_BASE(Law2_Dem3DofGeom_FrictPhys_Basic,LawFunctor);
		REGISTER_ATTRIBUTES(LawFunctor,/*nothing here*/);
};
REGISTER_SERIALIZABLE(Law2_Dem3DofGeom_FrictPhys_Basic);

/* Class for demonstrating beam-like behavior of the contact (normal, shear, bend and twist) */
class Law2_Dem6DofGeom_FrictPhys_Beam: public LawFunctor{
	public:
		virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, Scene*);
		FUNCTOR2D(Dem6DofGeom,FrictPhys);
		REGISTER_CLASS_AND_BASE(Law2_Dem6DofGeom_FrictPhys_Beam,LawFunctor);
		REGISTER_ATTRIBUTES(LawFunctor,/*nothing here*/);
};
REGISTER_SERIALIZABLE(Law2_Dem6DofGeom_FrictPhys_Beam);

class ElasticContactLaw : public InteractionSolver
{
/// Attributes
	private :
	public :
		int sdecGroupMask;
		bool momentRotationLaw;
		///Turn this true if another constitutive law is taking care of removing interactions :
		bool neverErase;
		#ifdef SCG_SHEAR
			bool useShear;
		#endif
	
		ElasticContactLaw();
		void action(Scene*);

		shared_ptr<Law2_ScGeom_FrictPhys_Basic> functor;

		REGISTER_ATTRIBUTES(InteractionSolver,(sdecGroupMask)(momentRotationLaw)(neverErase)
		#ifdef SCG_SHEAR
			(useShear)
		#endif
	);
	REGISTER_CLASS_NAME(ElasticContactLaw);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);
};

REGISTER_SERIALIZABLE(ElasticContactLaw);



