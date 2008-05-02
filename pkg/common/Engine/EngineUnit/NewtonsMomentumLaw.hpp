/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef NEWTONSMOMENTUMLAW_HPP
#define NEWTONSMOMENTUMLAW_HPP

#include "PhysicalActionApplierUnit.hpp"

class NewtonsMomentumLaw : public PhysicalActionApplierUnit
{
	public :
		virtual void go( 	  const shared_ptr<PhysicalAction>&
					, const shared_ptr<PhysicalParameters>&
					, const Body*);
	
	NEEDS_BEX("Momentum");
	FUNCTOR2D(Momentum,RigidBodyParameters);
	REGISTER_CLASS_NAME(NewtonsMomentumLaw);
	REGISTER_BASE_CLASS_NAME(PhysicalActionApplierUnit);
};

REGISTER_SERIALIZABLE(NewtonsMomentumLaw,false);

#endif // NEWTONSMOMENTUMLAW_HPP

