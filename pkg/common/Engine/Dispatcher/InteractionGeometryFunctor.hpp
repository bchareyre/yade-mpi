/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/Shape.hpp>
#include<yade/lib-base/Math.hpp>
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
	#ifdef YADE_DEVIRT_FUNCTORS
		// type of the pointer to devirtualized functor (static method taking the functor instance as the first argument)
		typedef bool(*StaticFuncPtr)(InteractionGeometryFunctor*, const shared_ptr<Shape>&, const shared_ptr<Shape>&, const State&, const State&, const Vector3r&, const bool&, const shared_ptr<Interaction>&);
		// return devirtualized functor (static method); must be overridden in derived classes
		virtual void* getStaticFuncPtr(){ throw runtime_error(("InteractionGeometryFunctor::getStaticFuncPtr() not overridden in class "+getClassName()+".").c_str()); }
	#endif
	YADE_CLASS_BASE_DOC(InteractionGeometryFunctor,Functor,"Functor for creating/updating :yref:`Interaction::interactionGeometry` objects.");
};
REGISTER_SERIALIZABLE(InteractionGeometryFunctor);


