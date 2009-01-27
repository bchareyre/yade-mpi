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

class ElasticContactLaw : public InteractionSolver
{
/// Attributes
	private :
		shared_ptr<PhysicalAction> actionForce;
		shared_ptr<PhysicalAction> actionMomentum;
		int actionForceIndex;
		int actionMomentumIndex;
		
	public :
		int sdecGroupMask;
		bool momentRotationLaw;
	
		ElasticContactLaw();
		void action(MetaBody*);

	protected :
		void registerAttributes();

	NEEDS_BEX("Force","Momentum");
	REGISTER_CLASS_NAME(ElasticContactLaw);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);
};

REGISTER_SERIALIZABLE(ElasticContactLaw);


