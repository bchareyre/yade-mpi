/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTIONPHYSICS_ENGINEUNIT_HPP
#define INTERACTIONPHYSICS_ENGINEUNIT_HPP

#include <yade/yade-core/PhysicalParameters.hpp>
#include <yade/yade-core/Interaction.hpp>
#include <yade/yade-core/EngineUnit2D.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

class InteractionPhysicsEngineUnit : 	public EngineUnit2D
					<
		 				void ,
		 				TYPELIST_3(	  const shared_ptr<PhysicalParameters>&
								, const shared_ptr<PhysicalParameters>&
								, const shared_ptr<Interaction>&
			   				  ) 
					>
{
	REGISTER_CLASS_NAME(InteractionPhysicsEngineUnit);
	REGISTER_BASE_CLASS_NAME(EngineUnit2D);

};

REGISTER_SERIALIZABLE(InteractionPhysicsEngineUnit,false);

#endif // INTERACTIONPHYSICS_ENGINEUNIT_HPP

