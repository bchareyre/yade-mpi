/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "AABB.hpp"

AABB::AABB(): BoundingVolume(), halfSize(0,0,0), center(0,0,0){
	createIndex();
}

AABB::~AABB ()
{
}

YADE_PLUGIN((AABB));


YADE_REQUIRE_FEATURE(PHYSPAR);

