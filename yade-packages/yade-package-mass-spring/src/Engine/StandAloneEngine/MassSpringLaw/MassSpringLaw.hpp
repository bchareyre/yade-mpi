/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef MASSSPRINGLAW_HPP
#define MASSSPRINGLAW_HPP

#include <yade/yade-core/Engine.hpp>
#include <yade/yade-core/PhysicalAction.hpp>

class MassSpringLaw : public Engine
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
	REGISTER_BASE_CLASS_NAME(Engine);
};

REGISTER_SERIALIZABLE(MassSpringLaw,false);

#endif // __EXPLICITMASSSPRINGDYNAMICENGINE_H__

