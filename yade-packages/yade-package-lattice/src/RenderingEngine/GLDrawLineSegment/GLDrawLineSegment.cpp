/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawLineSegment.hpp"
#include "LineSegment.hpp"
#include <yade/yade-lib-opengl/OpenGLWrapper.hpp>

void GLDrawLineSegment::go(const shared_ptr<GeometricalModel>& gm, const shared_ptr<PhysicalParameters>&,bool wire)
{
	// FIXME : check that : one of those 2 lines are useless
  	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gm->diffuseColor);
	glColor3v(gm->diffuseColor);
	
	Real len = (static_cast<LineSegment*>(gm.get()))->length;

	// FIXME - there must be a way to tell this from outside
//	glScalef(len,0.030,0.030); // it's a box, not a line. looks better :)
//	glScalef(len,0.010,0.010); // it's a box, not a line. looks better :)
//	glScalef(len,0.001,0.001); // it's a box, not a line. looks better :)
	glScalef(len,0.00007,0.00007); // it's a box, not a line. looks better :)

	if (gm->wire || wire)
	{
		glBegin(GL_LINES);
		glDisable(GL_LIGHTING);

		glVertex3(-0.5,0.0,0.0);
		glVertex3( 0.5,0.0,0.0);

//		glVertex3( 0.0,0.0,0.0);
//		glVertex3( 0.0, 0.4*len/0.042 ,0.0);

//		glVertex3( 0.0,0.0,0.0);
//		glVertex3( 0.0, 0.0 , 0.25*len/0.042 );

		glEnd();
	}
	else
	{
		glEnable(GL_LIGHTING);
		glutSolidCube(1);
	}
}

