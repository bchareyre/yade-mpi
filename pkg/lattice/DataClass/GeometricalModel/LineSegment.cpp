/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "LineSegment.hpp"

YADE_REQUIRE_FEATURE(shape);
LineSegment::LineSegment () : GeometricalModel()
{		
	createIndex();
}

LineSegment::~LineSegment ()
{		
}


YADE_PLUGIN((LineSegment));
