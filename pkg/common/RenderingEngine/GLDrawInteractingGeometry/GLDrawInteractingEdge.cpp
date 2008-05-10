/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawInteractingEdge.hpp"
#include <yade/pkg-common/InteractingEdge.hpp>
#include<yade/lib-opengl/OpenGLWrapper.hpp>



void GLDrawInteractingEdge::go(const shared_ptr<InteractingGeometry>& cm, const shared_ptr<PhysicalParameters>& ,bool)
{   
    InteractingEdge* e = static_cast<InteractingEdge*>(cm.get());



    glBegin(GL_LINES);

	glDisable(GL_LIGHTING);

	if (e->flat)
	    glColor3(1.0,1.0,0.0);
	else
	    glColor3(0.0,1.0,0.0);

	glVertex3(0,0,0);
	glVertex3v(e->edge);

	glColor3(1.0,0.0,0.0);

	glVertex3v(e->edge/2);
	glVertex3v(e->edge/2 + e->normal1);

	if (e->both)
	{
	    glVertex3v(e->edge/2);
	    glVertex3v(e->edge/2 + e->normal2);
	}

    glEnd();
}

YADE_PLUGIN();
