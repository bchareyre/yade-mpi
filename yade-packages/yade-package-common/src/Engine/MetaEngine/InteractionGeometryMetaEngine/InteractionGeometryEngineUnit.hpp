/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTIONGEOMETRYFUNCTOR_HPP
#define INTERACTIONGEOMETRYFUNCTOR_HPP

#include <yade/yade-core/InteractingGeometry.hpp>
#include <yade/yade-lib-wm3-math/Se3.hpp>
#include <yade/yade-core/Interaction.hpp>
#include <yade/yade-core/EngineUnit2D.hpp>

#include <boost/shared_ptr.hpp>
#include <string>

/*! \brief Abstract interface for all interaction functor.

	Every functions that describe interaction between two InteractionGeometries must derive from InteractionGeometryEngineUnit.
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

REGISTER_SERIALIZABLE(InteractionGeometryEngineUnit,false);

#endif // INTERACTIONGEOMETRYFUNCTOR_HPP

