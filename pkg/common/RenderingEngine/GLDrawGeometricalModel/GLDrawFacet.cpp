/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawFacet.hpp"
#include <yade/pkg-common/Facet.hpp>
#include<yade/lib-opengl/OpenGLWrapper.hpp>

void GLDrawFacet::go(const shared_ptr<GeometricalModel>& gm, const shared_ptr<PhysicalParameters>& ph,bool wire)
{
    Facet* facet = static_cast<Facet*>(gm.get());
    
    glColor3v(gm->diffuseColor); 


    if (gm->wire || wire)
    {
	glBegin(GL_LINE_LOOP);
	glDisable(GL_LIGHTING);

	glVertex3(0.0,0.0,0.0);
	for (int i=0, e=facet->vertices.size(); i<e; ++i)
	    glVertex3v(facet->vertices[i]);

	glEnd();
    }
    else
    {
	glDisable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glBegin(GL_POLYGON);

	glVertex3(0.0,0.0,0.0);
	for (int i=0, e=facet->vertices.size(); i<e; ++i)
	    glVertex3v(facet->vertices[i]);

	glEnd();
    }
}
YADE_PLUGIN();
