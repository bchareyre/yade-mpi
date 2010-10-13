/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Gl1_Facet.hpp"
#include <yade/pkg-common/Facet.hpp>
#include<yade/lib-opengl/OpenGLWrapper.hpp>

bool Gl1_Facet::normals=false;

void Gl1_Facet::go(const shared_ptr<Shape>& cm, const shared_ptr<State>& ,bool wire,const GLViewInfo&)
{   
	Facet* facet = static_cast<Facet*>(cm.get());
	const vector<Vector3r>& vertices = facet->vertices;
	const Vector3r* ne = facet->ne;
	const Real& icr = facet->icr;

	if(cm->wire || wire){
		// facet
		glBegin(GL_LINE_LOOP);
			glColor3v(normals ? Vector3r(1,0,0): cm->color);
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
			glVertex3(0.0,0.0,0.0); glVertex3v(Vector3r(icr*ne[0]));
			glVertex3(0.0,0.0,0.0);	glVertex3v(Vector3r(icr*ne[1]));
			glVertex3(0.0,0.0,0.0);	glVertex3v(Vector3r(icr*ne[2]));
		glEnd();
	} else {
		glDisable(GL_CULL_FACE); 
		Vector3r normal=(facet->vertices[1]-facet->vertices[0]).cross(facet->vertices[2]-facet->vertices[1]); normal.normalize();
		glColor3v(cm->color);
		glBegin(GL_TRIANGLES);
			glNormal3v(normal); // this makes every triangle different WRT the light direction; important!
			glVertex3v(facet->vertices[0]);
			glVertex3v(facet->vertices[1]);
			glVertex3v(facet->vertices[2]);
		glEnd();
	}
}

YADE_PLUGIN((Gl1_Facet));
YADE_REQUIRE_FEATURE(OPENGL)
