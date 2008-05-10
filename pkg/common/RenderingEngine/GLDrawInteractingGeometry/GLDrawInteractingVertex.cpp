/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawInteractingVertex.hpp"
#include <yade/pkg-common/InteractingVertex.hpp>
#include<yade/lib-opengl/OpenGLWrapper.hpp>



void GLDrawInteractingVertex::go(const shared_ptr<InteractingGeometry>& cm, const shared_ptr<PhysicalParameters>& ,bool)
{   
    InteractingVertex* v = static_cast<InteractingVertex*>(cm.get());

    glDisable(GL_LIGHTING);

    if ( v->flat )
	glColor3(1.0,0.0,0.0);
    else
	glColor3(0.0,1.0,0.0);

    for (int i=0, ei=v->normals.size(); i<ei; ++i)
    {
	glBegin(GL_LINES);
	    glVertex3(0,0,0);
	    glVertex3v(v->normals[i]);
	glEnd();
    }


    glBegin(GL_POINTS);  
      glVertex3(0.0,0.0,0.0); 
    glEnd();
}

YADE_PLUGIN();
