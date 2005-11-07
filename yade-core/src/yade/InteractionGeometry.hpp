/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTIONGEOMETRY_HPP
#define INTERACTIONGEOMETRY_HPP

#include <yade/yade-lib-serialization/Serializable.hpp>

class InteractionGeometry : public Serializable
{
	REGISTER_CLASS_NAME(InteractionGeometry);
	REGISTER_BASE_CLASS_NAME(Serializable);
};

REGISTER_SERIALIZABLE(InteractionGeometry,false);

#endif // INTERACTIONGEOMETRY_HPP

