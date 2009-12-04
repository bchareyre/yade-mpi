/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*  © 2007 Václav Šmilauer <eudoxos@arcig.cz>
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Tetrahedron.hpp"

YADE_REQUIRE_FEATURE(geometricalmodel);

Tetrahedron::Tetrahedron(): GeometricalModel()
{
	createIndex();
 	// FIXME: something like v(4,Vector3r(0,0,0)) should work...
	// FIXME - remove that line when we can move to C array[4]
	for(size_t i=0; i<4; i++) v.push_back(Vector3r(0,0,0));
}

Tetrahedron::Tetrahedron(const Vector3r& v0,const Vector3r& v1,const Vector3r& v2,const Vector3r& v3) : GeometricalModel()
{
	createIndex();
	// FIXME - we can move to C array[4] ...
	v.push_back(v0); v.push_back(v1); v.push_back(v2); v.push_back(v3);
//	v[0]=v0; v[1]=v1; v[2]=v2; v[3]=v3;
}

Tetrahedron::~Tetrahedron()
{
}

YADE_PLUGIN((Tetrahedron));


YADE_REQUIRE_FEATURE(PHYSPAR);

