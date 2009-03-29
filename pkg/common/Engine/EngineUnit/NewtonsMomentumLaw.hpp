/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/PhysicalActionApplierUnit.hpp>
#ifdef BEX_CONTAINER
	class NewtonsMomentumLaw: public PhysicalActionApplierUnit{
		public:
			virtual void go(const shared_ptr<PhysicalParameters>&, const Body*, MetaBody*);
			FUNCTOR1D(RigidBodyParameters);
			REGISTER_CLASS_AND_BASE(NewtonsMomentumLaw,PhysicalActionApplierUnit);
	};
#else
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
#endif
REGISTER_SERIALIZABLE(NewtonsMomentumLaw);


