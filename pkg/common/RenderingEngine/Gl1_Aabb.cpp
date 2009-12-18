/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"Gl1_Aabb.hpp"
#include<yade/lib-opengl/OpenGLWrapper.hpp>
#include<yade/pkg-common/Aabb.hpp>
#include<yade/core/Scene.hpp>

void Gl1_Aabb::go(const shared_ptr<Bound>& bv, Scene* scene){
	Aabb* aabb = static_cast<Aabb*>(bv.get());
	glColor3v(bv->diffuseColor);
	glTranslatev(scene->cell.wrapShearedPt(scene->cell.shearPt(.5*(aabb->min+aabb->max))));
	// glDisable(GL_LIGHTING); // ??
	if(scene->isPeriodic) glMultMatrixd(scene->cell._glShearMatrix);
	glScalev(aabb->max-aabb->min);
	glutWireCube(1);
}

YADE_PLUGIN((Gl1_Aabb));
YADE_REQUIRE_FEATURE(OPENGL)
