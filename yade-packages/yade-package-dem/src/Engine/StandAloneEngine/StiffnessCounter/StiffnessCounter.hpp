/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef ELASTIC_CONTACT_LAW2_HPP
#define ELASTIC_CONTACT_LAW2_HPP

#include <yade/yade-core/InteractionSolver.hpp>

#include <set>
#include <boost/tuple/tuple.hpp>

class PhysicalAction;


class StiffnessCounter : public InteractionSolver
{
/// Attributes
	private :
		shared_ptr<PhysicalAction> actionForce;
		shared_ptr<PhysicalAction> actionMomentum;
		shared_ptr<PhysicalAction> actionStiffness;
		

	public :
		unsigned int interval;
		int sdecGroupMask;
		bool momentRotationLaw;
	
		StiffnessCounter();
		void action(Body* body);
		virtual bool isActivated();

	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(StiffnessCounter);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);
};

REGISTER_SERIALIZABLE(StiffnessCounter,false);

#endif // ELASTIC_CONTACT_LAW_HPP

