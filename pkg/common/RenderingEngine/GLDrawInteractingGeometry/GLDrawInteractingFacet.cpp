/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawInteractingFacet.hpp"
#include <yade/pkg-common/InteractingFacet.hpp>
#include<yade/lib-opengl/OpenGLWrapper.hpp>



void GLDrawInteractingFacet::go(const shared_ptr<InteractingGeometry>& cm, const shared_ptr<PhysicalParameters>& ,bool)
{   
    InteractingFacet* facet = static_cast<InteractingFacet*>(cm.get());
    int N = facet->vertices.size();

    glDisable(GL_LIGHTING);

    // facet
    glBegin(GL_LINE_LOOP);
	glColor3(1.0,0.0,0.0);
	glVertex3(0.0,0.0,0.0);
	for (int i=0; i<N; ++i)
	    glVertex3v(facet->vertices[i]);
    glEnd();

    Vector3r b;

    // normals of edges
    glBegin(GL_LINES);
	glColor3(1.0,0.0,1.0);

	b = facet->vertices[0]/2;
	glVertex3v(b);
	glVertex3v(b+facet->edgeNormals[0]);

	b = facet->vertices[N-1]/2;
	glVertex3v(b);
	glVertex3v(b+facet->edgeNormals[N]);

	for (int i=1; i<N; ++i)
	{
	    b = facet->vertices[i-1]+(facet->vertices[i]-facet->vertices[i-1])/2;
	    glVertex3v(b);
	    glVertex3v(b+facet->edgeNormals[i]);
	}
    glEnd();
    
    // normal of facet
    glBegin(GL_LINES);
	glColor3(0.0,0.0,1.0); 
	glVertex3v(facet->cm);
	glVertex3v(facet->cm+facet->normal);
    glEnd();
}

YADE_PLUGIN();
