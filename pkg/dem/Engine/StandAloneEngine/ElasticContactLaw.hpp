/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/InteractionSolver.hpp>
#include<yade/core/PhysicalAction.hpp>

// only to see whether SCG_SHEAR is defined, may be removed in the future
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-common/ConstitutiveLaw.hpp>

#include <set>
#include <boost/tuple/tuple.hpp>

class PhysicalAction;

class ElasticContactLaw2: public InteractionSolver{
	public:
	//! this should really be property of the interaction, but for simplicity keep it here now...
	bool isCohesive;
	ElasticContactLaw2();
	virtual ~ElasticContactLaw2();
	void action(MetaBody*);
	void registerAttributes(){
		InteractionSolver::registerAttributes();
		REGISTER_ATTRIBUTE(isCohesive);
	}
	NEEDS_BEX("Force","Momentum");
	REGISTER_CLASS_NAME(ElasticContactLaw2);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);
};
REGISTER_SERIALIZABLE(ElasticContactLaw2);

class ef2_Spheres_Elastic_ElasticLaw: public ConstitutiveLaw{
	public:
	virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, MetaBody* rootBody);
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
};
REGISTER_SERIALIZABLE(ef2_Spheres_Elastic_ElasticLaw);

class ElasticContactLaw : public InteractionSolver
{
/// Attributes
	private :
	#ifndef BEX_CONTAINER
		shared_ptr<PhysicalAction> actionForce;
		shared_ptr<PhysicalAction> actionMomentum;
		int actionForceIndex;
		int actionMomentumIndex;
		NEEDS_BEX("Force","Momentum");
	#endif
		
	public :
		int sdecGroupMask;
		bool momentRotationLaw;
		#ifdef SCG_SHEAR
			bool useShear;
		#endif
	
		ElasticContactLaw();
		void action(MetaBody*);

		shared_ptr<ef2_Spheres_Elastic_ElasticLaw> functor;

	protected :
		void registerAttributes();

	REGISTER_CLASS_NAME(ElasticContactLaw);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);
};

REGISTER_SERIALIZABLE(ElasticContactLaw);


