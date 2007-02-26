/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawInteractingMyTetrahedron.hpp"
#include "InteractingMyTetrahedron.hpp"
#include <yade/yade-lib-opengl/OpenGLWrapper.hpp>

void GLDrawInteractingMyTetrahedron::go(const shared_ptr<InteractingGeometry>& cm, const shared_ptr<PhysicalParameters>& ,bool)
{
	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Vector3f(cm->diffuseColor[0],cm->diffuseColor[1],cm->diffuseColor[2]));
	glColor3v(cm->diffuseColor);

	InteractingMyTetrahedron* t = static_cast<InteractingMyTetrahedron*>(cm.get());

	for(int i=0 ; i<4 ; ++i )
	{
		glPushMatrix();
		Vector3r& pos = (&(t->c1))[i];
		glTranslatef(pos[0],pos[1],pos[2]);
		float radius = (&(t->r1))[i];
	//	if (cm->wire)
	//	{
	//		glScalef(radius,radius,radius);
	//		glutWireSphere(1,5,5);
	//	}
	//	else
		{
			glScalef(radius,radius,radius);
			glutSolidSphere(1,8,8);
		}
		glPopMatrix();
	}
}

