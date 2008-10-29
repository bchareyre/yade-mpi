/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "BssSweptSphereLineSegment.hpp"


BssSweptSphereLineSegment::BssSweptSphereLineSegment () : InteractingGeometry()
{		
	createIndex();
        position = orientation = Vector3r(0,0,0);
        radius = length = 0.0;
}

BssSweptSphereLineSegment::~BssSweptSphereLineSegment ()
{		
}

void BssSweptSphereLineSegment::registerAttributes()
{
	InteractingGeometry::registerAttributes();
	REGISTER_ATTRIBUTE(radius);
        REGISTER_ATTRIBUTE(length);
        REGISTER_ATTRIBUTE(position);
        REGISTER_ATTRIBUTE(orientation);
}

YADE_PLUGIN();
