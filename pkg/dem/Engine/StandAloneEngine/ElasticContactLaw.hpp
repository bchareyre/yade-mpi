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
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-common/ConstitutiveLaw.hpp>

#include <set>
#include <boost/tuple/tuple.hpp>


class ef2_Spheres_Elastic_ElasticLaw: public ConstitutiveLaw{
	public:
	virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, MetaBody* rootBody, bool neverErase=false);
	int sdecGroupMask;
	bool momentRotationLaw;	
	
	#ifdef SCG_SHEAR
		bool useShear;
	#endif
	ef2_Spheres_Elastic_ElasticLaw(): sdecGroupMask(1), momentRotationLaw(true)
		#ifdef SCG_SHEAR
			, useShear(false)
		#endif
		{}
	FUNCTOR2D(SpheresContactGeometry,ElasticContactInteraction);
	REGISTER_CLASS_AND_BASE(ef2_Spheres_Elastic_ElasticLaw,ConstitutiveLaw);
	REGISTER_ATTRIBUTES(ConstitutiveLaw,(sdecGroupMask)(momentRotationLaw)
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
class Law2_Dem3Dof_Elastic_Elastic: public ConstitutiveLaw{
	public:
		virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, MetaBody* rootBody);
		FUNCTOR2D(Dem3DofGeom,ElasticContactInteraction);
		REGISTER_CLASS_AND_BASE(Law2_Dem3Dof_Elastic_Elastic,ConstitutiveLaw);
		REGISTER_ATTRIBUTES(ConstitutiveLaw,/*nothing here*/);
};
REGISTER_SERIALIZABLE(Law2_Dem3Dof_Elastic_Elastic);

class ElasticContactLaw : public InteractionSolver
{
/// Attributes
	private :
	public :
		int sdecGroupMask;
		bool momentRotationLaw;
		#ifdef SCG_SHEAR
			bool useShear;
		#endif
			
		/*! Turn this true if another law is taking care of removing interaction.
		 */
		bool neverErase;
	
		ElasticContactLaw();
		void action(MetaBody*);

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



