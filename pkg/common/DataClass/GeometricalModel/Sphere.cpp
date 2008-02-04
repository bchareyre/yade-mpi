/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Sphere.hpp"
#include<yade/lib-opengl/OpenGLWrapper.hpp>

Sphere::Sphere () : GeometricalModel()
{
	createIndex();
}


Sphere::~Sphere ()
{
}


void Sphere::registerAttributes()
{
	GeometricalModel::registerAttributes();
	REGISTER_ATTRIBUTE(radius);
}

YADE_PLUGIN();
