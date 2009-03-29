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
	class NewtonsForceLaw: public PhysicalActionApplierUnit{
		public:
			virtual void go(const shared_ptr<PhysicalParameters>&, const Body*, MetaBody*);
			FUNCTOR1D(ParticleParameters);
			REGISTER_CLASS_AND_BASE(NewtonsForceLaw,PhysicalActionApplierUnit);
	};

#else
	class NewtonsForceLaw : public PhysicalActionApplierUnit
	{
		public :
			virtual void go( 	  const shared_ptr<PhysicalAction>&
						, const shared_ptr<PhysicalParameters>&
						, const Body*);
		NEEDS_BEX("Force");
		FUNCTOR2D(Force,ParticleParameters);
		REGISTER_CLASS_NAME(NewtonsForceLaw);
		REGISTER_BASE_CLASS_NAME(PhysicalActionApplierUnit);
	};
#endif

REGISTER_SERIALIZABLE(NewtonsForceLaw);


