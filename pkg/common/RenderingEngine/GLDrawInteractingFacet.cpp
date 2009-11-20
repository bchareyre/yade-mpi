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

bool GLDrawInteractingFacet::normals=false;

void GLDrawInteractingFacet::go(const shared_ptr<InteractingGeometry>& cm, const shared_ptr<PhysicalParameters>& ,bool,const GLViewInfo&)
{   
    InteractingFacet* facet = static_cast<InteractingFacet*>(cm.get());
    const vector<Vector3r>& vertices = facet->vertices;
	const Vector3r* ne = facet->ne;
	const Real& icr = facet->icr;

  //  glDisable(GL_LIGHTING);

    // facet
    glBegin(GL_LINE_LOOP);
	glColor3(1.0,0.0,0.0);
	    glVertex3v(vertices[0]);
	    glVertex3v(vertices[1]);
	    glVertex3v(vertices[2]);
    glEnd();

	 if(!normals) return;

	// facet's normal 
    glBegin(GL_LINES);
		glColor3(0.0,0.0,1.0); 
		glVertex3(0.0,0.0,0.0);
		glVertex3v(facet->nf);
    glEnd();

    // normal of edges
	glColor3(0.0,0.0,1.0); 
    glBegin(GL_LINES);
		glVertex3(0.0,0.0,0.0);
		glVertex3v(icr*ne[0]);

		glVertex3(0.0,0.0,0.0);
		glVertex3v(icr*ne[1]);

		glVertex3(0.0,0.0,0.0);
		glVertex3v(icr*ne[2]);
    glEnd();
}

YADE_PLUGIN((GLDrawInteractingFacet));
YADE_REQUIRE_FEATURE(OPENGL)
