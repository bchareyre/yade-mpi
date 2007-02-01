/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SPRINGPHYSICS_HPP
#define SPRINGPHYSICS_HPP

#include <yade/yade-core/InteractionPhysics.hpp>

class SpringPhysics : public InteractionPhysics
{
	public :
		Real		 stiffness
				,damping
				,initialLength;

		SpringPhysics();
		virtual ~SpringPhysics();

	protected :
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(SpringPhysics);
	REGISTER_BASE_CLASS_NAME(InteractionPhysics);

	REGISTER_CLASS_INDEX(SpringPhysics,InteractionPhysics);
};

REGISTER_SERIALIZABLE(SpringPhysics,false);

#endif // SPRINGPHYSICS_HPP

