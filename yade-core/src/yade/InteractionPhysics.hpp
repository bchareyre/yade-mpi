/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTIONPHYSICS_HPP
#define INTERACTIONPHYSICS_HPP

#include <yade/yade-lib-base/yadeWm3Extra.hpp>
#include <yade/yade-lib-serialization/Serializable.hpp>
#include <yade/yade-lib-multimethods/Indexable.hpp>

class InteractionPhysics : public Serializable, public Indexable
{
	REGISTER_CLASS_NAME(InteractionPhysics);
	REGISTER_BASE_CLASS_NAME(Serializable Indexable);

	REGISTER_INDEX_COUNTER(InteractionPhysics);
};

REGISTER_SERIALIZABLE(InteractionPhysics,false);

#endif //  INTERACTIONPHYSICS_HPP

