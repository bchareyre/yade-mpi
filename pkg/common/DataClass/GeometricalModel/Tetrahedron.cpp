/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*  © 2007 Václav Šmilauer <eudoxos@arcig.cz>
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Tetrahedron.hpp"


Tetrahedron::Tetrahedron(): GeometricalModel()
{
	createIndex();
	// FIXME: something like v(4,Vector3r(0,0,0)) should work...
	for(size_t i=0; i<4; i++) v.push_back(Vector3r(0,0,0));
}

Tetrahedron::Tetrahedron(Vector3r& v1,Vector3r& v2,Vector3r& v3,Vector3r& v4) : GeometricalModel()
{
	createIndex();
	v.push_back(v1); v.push_back(v2); v.push_back(v3); v.push_back(v4);
}

Tetrahedron::~Tetrahedron()
{
}

void Tetrahedron::registerAttributes()
{
	GeometricalModel::registerAttributes();
	// FIXME
	REGISTER_ATTRIBUTE(v);
}
