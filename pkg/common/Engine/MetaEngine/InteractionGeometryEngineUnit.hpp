/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/InteractingGeometry.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/core/EngineUnit.hpp>
#include<yade/core/State.hpp>

#include <string>

/*! \brief
	Abstract interface for all classes that build Interaction from two interacting InteractingGeometry 'ies

	This is a crucial EngineUnit used during runtime, when an interaction is detected and starts to exist between two
	Body 'ies, a class Interaction to describe it must be created.

	\param const shared_ptr<InteractingGeometry>&	first Body geometry
	\param const shared_ptr<InteractingGeometry>&	second Body geometry
	\param State&					first Body's State
	\param State&					second Body's State
	\param Vector3r& 				second Body's relative shift (for periodicity)
	\return shared_ptr<Interaction>&		it returns the Interaction to be built (given as last argument to the function)
	
*/

class InteractionGeometryEngineUnit : 	public EngineUnit2D
					<
		 				bool ,
		 				TYPELIST_6(	  const shared_ptr<InteractingGeometry>&
								, const shared_ptr<InteractingGeometry>&
								, const State&
								, const State&
								, const Vector3r& 
								, const shared_ptr<Interaction>&
			  				  ) 
					>
{
	public: virtual ~InteractionGeometryEngineUnit();
	REGISTER_CLASS_AND_BASE(InteractionGeometryEngineUnit,EngineUnit2D);
	REGISTER_ATTRIBUTES(EngineUnit,/* no attributes here */);
};

REGISTER_SERIALIZABLE(InteractionGeometryEngineUnit);


