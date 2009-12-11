/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Gl1_Sphere.hpp"
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/lib-opengl/OpenGLWrapper.hpp>

bool Gl1_Sphere::wire=false;
bool Gl1_Sphere::glutNormalize=true;
int  Gl1_Sphere::glutSlices=12;
int  Gl1_Sphere::glutStacks=6;

void Gl1_Sphere::go(const shared_ptr<Shape>& cm, const shared_ptr<State>& ,bool wire2, const GLViewInfo&)
{
	Real r=(static_cast<InteractingSphere*>(cm.get()))->radius;
	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Vector3f(cm->diffuseColor[0],cm->diffuseColor[1],cm->diffuseColor[2]));
	glColor3v(cm->diffuseColor);
	if(glutNormalize)	glPushAttrib(GL_NORMALIZE); // as per http://lists.apple.com/archives/Mac-opengl/2002/Jul/msg00085.html
	 	if (wire || wire2) glutWireSphere(r,glutSlices,glutStacks);
		else glutSolidSphere(r,glutSlices,glutStacks);
	if(glutNormalize) glPopAttrib();
	return;
}
YADE_PLUGIN((Gl1_Sphere));
YADE_REQUIRE_FEATURE(OPENGL)
