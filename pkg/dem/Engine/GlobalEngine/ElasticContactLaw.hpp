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


class ef2_Spheres_Elastic_ElasticLaw: public LawFunctor{
	public:
	virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, Scene* rootBody);
	int sdecGroupMask;
	bool momentRotationLaw;	
	bool neverErase;	

	#ifdef SCG_SHEAR
		bool useShear;
	#endif
	
	ef2_Spheres_Elastic_ElasticLaw(): sdecGroupMask(1), momentRotationLaw(true), neverErase(false)
		#ifdef SCG_SHEAR
			, useShear(false)
		#endif
		{}
	FUNCTOR2D(ScGeom,ElasticContactInteraction);
	REGISTER_CLASS_AND_BASE(ef2_Spheres_Elastic_ElasticLaw,LawFunctor);
	REGISTER_ATTRIBUTES(LawFunctor,(sdecGroupMask)(momentRotationLaw)(neverErase)
		#ifdef SCG_SHEAR
			(useShear)
		#endif
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(ef2_Spheres_Elastic_ElasticLaw);

/* Constitutive law for linear compression, no tension, and linear plasticity surface.

This class serves also as tutorial and is documented in detail at

	http://yade.wikia.com/wiki/ConstitutiveLawHowto
*/
class Law2_Dem3Dof_Elastic_Elastic: public LawFunctor{
	public:
		virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, Scene* rootBody);
		FUNCTOR2D(Dem3DofGeom,ElasticContactInteraction);
		REGISTER_CLASS_AND_BASE(Law2_Dem3Dof_Elastic_Elastic,LawFunctor);
		REGISTER_ATTRIBUTES(LawFunctor,/*nothing here*/);
};
REGISTER_SERIALIZABLE(Law2_Dem3Dof_Elastic_Elastic);

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

		shared_ptr<ef2_Spheres_Elastic_ElasticLaw> functor;

		REGISTER_ATTRIBUTES(InteractionSolver,(sdecGroupMask)(momentRotationLaw)(neverErase)
		#ifdef SCG_SHEAR
			(useShear)
		#endif
	);
	REGISTER_CLASS_NAME(ElasticContactLaw);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);
};

REGISTER_SERIALIZABLE(ElasticContactLaw);



