/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawVertex.hpp"
#include <yade/pkg-common/Vertex.hpp>
#include<yade/lib-opengl/OpenGLWrapper.hpp>

void GLDrawVertex::go(const shared_ptr<GeometricalModel>& gm, const shared_ptr<PhysicalParameters>& ph,bool wire)
{
    glColor3v(gm->diffuseColor); 
    glBegin(GL_POINTS);  
      glDisable(GL_LIGHTING);
      glVertex3(0.0,0.0,0.0); 
    glEnd();
}
YADE_PLUGIN();
