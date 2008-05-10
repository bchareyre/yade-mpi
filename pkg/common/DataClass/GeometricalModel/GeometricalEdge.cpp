/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GeometricalEdge.hpp"
#include<yade/lib-opengl/OpenGLWrapper.hpp>

GeometricalEdge::GeometricalEdge () : GeometricalModel()
{
	createIndex();
}


GeometricalEdge::~GeometricalEdge ()
{
}


void GeometricalEdge::registerAttributes()
{
	GeometricalModel::registerAttributes();
	REGISTER_ATTRIBUTE(edge);
}

YADE_PLUGIN();
