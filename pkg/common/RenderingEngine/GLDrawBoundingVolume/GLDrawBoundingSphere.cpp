/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawBoundingSphere.hpp"
#include "BoundingSphere.hpp"
#include<yade/lib-opengl/OpenGLWrapper.hpp>

void GLDrawBoundingSphere::go(const shared_ptr<BoundingVolume>& bv)
{
	BoundingSphere * s = static_cast<BoundingSphere*>(bv.get());
	glColor3v(bv->diffuseColor);
	glTranslate(s->center[0],s->center[1],s->center[2]);
	glutWireSphere(s->radius,10,10);
}

