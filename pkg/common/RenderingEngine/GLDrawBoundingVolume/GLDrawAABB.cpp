/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawAABB.hpp"
#include "AABB.hpp"


#include<yade/lib-opengl/OpenGLWrapper.hpp>


void GLDrawAABB::go(const shared_ptr<BoundingVolume>& bv)
{
	AABB * aabb = static_cast<AABB*>(bv.get());
	glColor3v(bv->diffuseColor);
	glTranslate(aabb->center[0],aabb->center[1],aabb->center[2]);
	glScale(2*aabb->halfSize[0],2*aabb->halfSize[1],2*aabb->halfSize[2]);
	glDisable(GL_LIGHTING);
	glutWireCube(1);
}

