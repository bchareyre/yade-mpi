/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Tetrahedron.hpp"


Tetrahedron::Tetrahedron() : GeometricalModel()
{
	createIndex();
}

Tetrahedron::Tetrahedron(Vector3r& p1,Vector3r& p2,Vector3r& p3,Vector3r& p4) : GeometricalModel()
{
	createIndex();
	v1 = p1;
	v2 = p2;
	v3 = p3;
	v4 = p4;
}

Tetrahedron::~Tetrahedron()
{
}

void Tetrahedron::registerAttributes()
{
	GeometricalModel::registerAttributes();
	// FIXME
	REGISTER_ATTRIBUTE(v1); // no need to save them (?)
	REGISTER_ATTRIBUTE(v2); // no need to save them (?)
	REGISTER_ATTRIBUTE(v3); // no need to save them (?)
	REGISTER_ATTRIBUTE(v4); // no need to save them (?)
}
