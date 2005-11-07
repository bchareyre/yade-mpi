/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef ACTION_FORCE_DAMPING_HPP
#define ACTION_FORCE_DAMPING_HPP

#include "PhysicalActionDamperUnit.hpp"

class CundallNonViscousForceDamping : public PhysicalActionDamperUnit
{
	public :
		Real	damping;

		CundallNonViscousForceDamping();
	
		virtual void go( 	  const shared_ptr<PhysicalAction>&
					, const shared_ptr<PhysicalParameters>&
					, const Body*);
	
	protected :
		virtual void registerAttributes();

	REGISTER_CLASS_NAME(CundallNonViscousForceDamping);
	REGISTER_BASE_CLASS_NAME(PhysicalActionDamperUnit);
};

REGISTER_SERIALIZABLE(CundallNonViscousForceDamping,false);

#endif // ACTION_FORCE_DAMPING_HPP
