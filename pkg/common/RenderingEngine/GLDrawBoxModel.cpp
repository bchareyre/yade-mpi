/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawBoxModel.hpp"
#include<yade/pkg-common/BoxModel.hpp>


#include<yade/lib-opengl/OpenGLWrapper.hpp>

YADE_REQUIRE_FEATURE(geometricalmodel);

void GLDrawBoxModel::go(const shared_ptr<GeometricalModel>& gm, const shared_ptr<PhysicalParameters>&,bool wire)
{
	// FIXME : check that : one of those 2 lines are useless
  	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Vector3f(gm->diffuseColor[0],gm->diffuseColor[1],gm->diffuseColor[2]));
	glColor3v(gm->diffuseColor);
	
	Vector3r &extents = (static_cast<BoxModel*>(gm.get()))->extents;
	
	glScalef(2*extents[0],2*extents[1],2*extents[2]);

	if (wire)
	{
		glDisable(GL_LIGHTING);
		glutWireCube(1);
	}
	else
	{
		glEnable(GL_LIGHTING);
		glutSolidCube(1);
	}
}


YADE_PLUGIN((GLDrawBoxModel));
YADE_REQUIRE_FEATURE(OPENGL)

YADE_REQUIRE_FEATURE(PHYSPAR);

