/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/Shape.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/core/Functor.hpp>
#include<yade/core/State.hpp>

#include <string>

/*! \brief
	Abstract interface for all classes that build Interaction from two interacting Shape 'ies

	This is a crucial Functor used during runtime, when an interaction is detected and starts to exist between two
	Body 'ies, a class Interaction to describe it must be created.

	\param const shared_ptr<Shape>&	first Body geometry
	\param const shared_ptr<Shape>&	second Body geometry
	\param State&					first Body's State
	\param State&					second Body's State
	\param Vector3r& 				second Body's relative shift (for periodicity)
	\param bool & force        force creation of the geometry, even if the interaction doesn't exist yet and the bodies are too far for regular contact (used from explicitAction)
	\return shared_ptr<Interaction>&		it returns the Interaction to be built (given as last argument to the function)
	
*/

class InteractionGeometryFunctor : 	public Functor2D
					<
		 				bool ,
		 				TYPELIST_7(	  const shared_ptr<Shape>&
								, const shared_ptr<Shape>&
								, const State&
								, const State&
								, const Vector3r& 
								, const bool&
								, const shared_ptr<Interaction>&
			  				  ) 
					>
{
	public: virtual ~InteractionGeometryFunctor();
	//REGISTER_CLASS_AND_BASE(InteractionGeometryFunctor,Functor2D);
	//REGISTER_ATTRIBUTES(Functor,/* no attributes here */);
	YADE_CLASS_BASE_ATTRS(InteractionGeometryFunctor,Functor,/*no attrs*/);
};
REGISTER_SERIALIZABLE(InteractionGeometryFunctor);


