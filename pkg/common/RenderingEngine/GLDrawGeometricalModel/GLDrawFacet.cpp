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
    
    
	if (gm->wire || wire)
    {
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
	//	glBegin(GL_TRIANGLES);
		glDisable(GL_LIGHTING);
		glBegin(GL_LINE_LOOP);
			glColor3v(gm->diffuseColor); 
			glVertex3v(facet->vertices[0]);
			glVertex3v(facet->vertices[1]);
			glVertex3v(facet->vertices[2]);
		glEnd();
    }
    else
    {
		glDisable(GL_CULL_FACE); 
  		glMaterialv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,gm->diffuseColor); glColor3v(gm->diffuseColor); // one of those necessary as well
		glEnable(GL_LIGHTING); // important
		Vector3r normal=(facet->vertices[1]-facet->vertices[0]).Cross(facet->vertices[2]-facet->vertices[1]); normal.Normalize();
		glBegin(GL_TRIANGLES);
			glNormal3v(normal); // this makes every triangle different WRT the light direction; important!
			glVertex3v(facet->vertices[0]);
			glVertex3v(facet->vertices[1]);
			glVertex3v(facet->vertices[2]);
		glEnd();
    }
}
YADE_PLUGIN((GLDrawFacet));
YADE_REQUIRE_FEATURE(OPENGL)
