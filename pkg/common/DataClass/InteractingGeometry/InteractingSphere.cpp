/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractingSphere.hpp"

InteractingSphere::InteractingSphere () : InteractingGeometry(), radius(0.0)
{
	createIndex();
}

InteractingSphere::~InteractingSphere ()
{
}


YADE_PLUGIN((InteractingSphere));

