/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTIONGEOMETRYFUNCTOR_HPP
#define INTERACTIONGEOMETRYFUNCTOR_HPP

#include<yade/core/InteractingGeometry.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/core/EngineUnit2D.hpp>

#include <string>

/*! \brief
	Abstract interface for all classes that build Interaction from two interacting InteractingGeometry 'ies

	This is a crucial EngineUnit used during runtime, when an interaction is detected and starts to exist between two
	Body 'ies, a class Interaction to describe it must be created.
	 
	Currently we can build: SpheresContactGeometry, ErrorTolerantContact, ClosestFeatures

	\param const shared_ptr<InteractingGeometry>&	first interacting Body
	\param const shared_ptr<InteractingGeometry>&	second interacting Body
	\param Se3r&					first Body's 3d transformation (FIXME: should be removed)
	\param Se3r&					second Body's 3d transformation (FIXME: should be removed)
	\return shared_ptr<Interaction>&		it returns the Interaction to be built (given as last argument to the function)
	
*/

class InteractionGeometryEngineUnit : 	public EngineUnit2D
					<
		 				bool ,
		 				TYPELIST_5(	  const shared_ptr<InteractingGeometry>&
								, const shared_ptr<InteractingGeometry>&
								, const Se3r&
								, const Se3r&
								, const shared_ptr<Interaction>&
			  				  ) 
					>
{
	REGISTER_CLASS_NAME(InteractionGeometryEngineUnit);
	REGISTER_BASE_CLASS_NAME(EngineUnit2D);
};

REGISTER_SERIALIZABLE(InteractionGeometryEngineUnit);

#endif // INTERACTIONGEOMETRYFUNCTOR_HPP

