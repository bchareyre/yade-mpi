/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawGeometricalEdge.hpp"
#include <yade/pkg-common/GeometricalEdge.hpp>
#include<yade/lib-opengl/OpenGLWrapper.hpp>

void GLDrawGeometricalEdge::go(const shared_ptr<GeometricalModel>& gm, const shared_ptr<PhysicalParameters>& ph,bool wire)
{
	Vector3r edge = static_cast<GeometricalEdge*>(gm.get())->edge;

	glColor3v(gm->diffuseColor); 

	glBegin(GL_LINES);
	glDisable(GL_LIGHTING);

	glVertex3(0.0,0.0,0.0);
	glVertex3v(edge);

	glEnd();
}
YADE_PLUGIN();
