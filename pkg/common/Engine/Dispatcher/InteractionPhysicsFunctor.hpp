/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/Material.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/core/Functor.hpp>
#include <string>

//! Abstract interface for all classes that build InteractionPhysics from two interacting Body's PhysicalParameters
class InteractionPhysicsFunctor: public Functor2D
					<
		 				void ,
		 				TYPELIST_3(	  const shared_ptr<Material>&
								, const shared_ptr<Material>&
								, const shared_ptr<Interaction>&
			   				  ) 
					>
{
	public: virtual ~InteractionPhysicsFunctor();
	YADE_CLASS_BASE_DOC(InteractionPhysicsFunctor,Functor,"Functor for creating/updating :yref:`Interaction::interactionPhysics` objects.");

};
REGISTER_SERIALIZABLE(InteractionPhysicsFunctor);


