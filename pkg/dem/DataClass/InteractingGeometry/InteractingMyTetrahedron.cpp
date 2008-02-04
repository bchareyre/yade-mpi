/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractingMyTetrahedron.hpp"

InteractingMyTetrahedron::InteractingMyTetrahedron () : InteractingGeometry()
{
	createIndex();
	c1=c2=c3=c4=Vector3r(0,0,0);
	r1=r2=r3=r4=0;
}

InteractingMyTetrahedron::~InteractingMyTetrahedron ()
{
}

void InteractingMyTetrahedron::registerAttributes()
{
	InteractingGeometry::registerAttributes();
	REGISTER_ATTRIBUTE(c1);
	REGISTER_ATTRIBUTE(c2);
	REGISTER_ATTRIBUTE(c3);
	REGISTER_ATTRIBUTE(c4);
	REGISTER_ATTRIBUTE(r1);
	REGISTER_ATTRIBUTE(r2);
	REGISTER_ATTRIBUTE(r3);
	REGISTER_ATTRIBUTE(r4);
}


YADE_PLUGIN();
