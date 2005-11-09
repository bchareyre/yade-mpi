/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef MASSSPRINGLAW_HPP
#define MASSSPRINGLAW_HPP

#include <yade/yade-core/InteractionSolver.hpp>
#include <yade/yade-core/PhysicalAction.hpp>

class MassSpringLaw : public InteractionSolver
{
	private :
		shared_ptr<PhysicalAction> actionForce;	
		shared_ptr<PhysicalAction> actionMomentum;

	public :
		int springGroupMask;
		MassSpringLaw ();
		void action(Body* body);

	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(MassSpringLaw);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);
};

REGISTER_SERIALIZABLE(MassSpringLaw,false);

#endif // MASSSPRINGLAW_HPP

