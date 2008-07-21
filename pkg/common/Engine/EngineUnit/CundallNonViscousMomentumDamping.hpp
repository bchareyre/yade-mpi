/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef ACTION_MOMENTUM_DAMPING_HPP
#define ACTION_MOMENTUM_DAMPING_HPP

#include<yade/pkg-common/PhysicalActionDamperUnit.hpp>

class CundallNonViscousMomentumDamping : public PhysicalActionDamperUnit
{
	public :
		Real	damping;

		CundallNonViscousMomentumDamping();
	
		virtual void go( 	  const shared_ptr<PhysicalAction>&
					, const shared_ptr<PhysicalParameters>&
					, const Body*);
	
	protected :
		 virtual void registerAttributes();

	NEEDS_BEX("Momentum");
	FUNCTOR2D(Momentum,RigidBodyParameters);
	REGISTER_CLASS_NAME(CundallNonViscousMomentumDamping);
	REGISTER_BASE_CLASS_NAME(PhysicalActionDamperUnit);
};

REGISTER_SERIALIZABLE(CundallNonViscousMomentumDamping,false);

#endif // ACTION_MOMENTUM_DAMPING_HPP
