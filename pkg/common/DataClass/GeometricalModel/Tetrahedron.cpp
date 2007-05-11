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
}

Tetrahedron::Tetrahedron(const Vector3r& v0,const Vector3r& v1,const Vector3r& v2,const Vector3r& v3) : GeometricalModel()
{
	createIndex();
	v[0]=v0; v[1]=v1; v[2]=v2; v[3]=v3;
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
