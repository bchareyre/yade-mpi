/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLOBALSTIFFNESSCOUNTER_HPP
#define GLOBALSTIFFNESSCOUNTER_HPP

#include <yade/yade-core/InteractionSolver.hpp>

#include <set>
#include <boost/tuple/tuple.hpp>

class PhysicalAction;


class GlobalStiffnessCounter : public InteractionSolver
{
/// Attributes
	private :
		shared_ptr<PhysicalAction> actionForce;
		shared_ptr<PhysicalAction> actionMomentum;
		shared_ptr<PhysicalAction> actionStiffness;
		int actionForceIndex;
		int actionMomentumIndex;
		int actionStiffnessIndex;
		
		

	public :
		unsigned int interval;
		int sdecGroupMask;
		bool momentRotationLaw;
	
		GlobalStiffnessCounter();
		void action(Body* body);
		virtual bool isActivated();

	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(GlobalStiffnessCounter);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);
};

REGISTER_SERIALIZABLE(GlobalStiffnessCounter,false);

#endif // GLOBALSTIFFNESSCOUNTER_HPP

