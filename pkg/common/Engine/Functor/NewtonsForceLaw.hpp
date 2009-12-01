/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/PhysicalActionApplierUnit.hpp>

class NewtonsForceLaw: public PhysicalActionApplierUnit{
	public:
		virtual void go(const shared_ptr<State>&, const Body*, World*);
		FUNCTOR1D(ParticleParameters);
		REGISTER_CLASS_AND_BASE(NewtonsForceLaw,PhysicalActionApplierUnit);
};
REGISTER_SERIALIZABLE(NewtonsForceLaw);


