/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawSphereShadowVolume.hpp"
#include "Sphere.hpp"
#include<yade/lib-opengl/OpenGLWrapper.hpp>


void GLDrawSphereShadowVolume::go(const shared_ptr<GeometricalModel>& gm , const shared_ptr<PhysicalParameters>& pp, const Vector3r& lightPos)
{
	//FIXME : do not cast on RigidBodyParameters but use Parameters instead
	//Vector3r center = (static_cast<ParticleParameters*>(pp.get()))->se3.position;
	Vector3r center = pp->se3.position;
	Real radius = (static_cast<Sphere*>(gm.get()))->radius;
	
	Vector3r dir = lightPos-center;
	Vector3r normalDir(-dir[1],dir[0],0);
	normalDir.Normalize();
	normalDir *= radius;
	Vector3r biNormalDir = normalDir.UnitCross(dir)*radius;
	
	int nbSegments = 15;
	
	Vector3r p1,p2;
	glBegin(GL_QUAD_STRIP);
		p1 = center+biNormalDir;
		p2 = p1 + (p1-lightPos)*10;
		glVertex3v(p1);
		glVertex3v(p2);
		for(int i=1;i<=nbSegments;i++)
		{
			Real angle = Mathr::TWO_PI/(Real)nbSegments*i;
			p1 = center+sin(angle)*normalDir+cos(angle)*biNormalDir;
			p2 = p1 + (p1-lightPos)*10;
			glVertex3v(p1);
			glVertex3v(p2);
		}
	glEnd();
			
	// closing shadow volumes ??
	// 	glColor3f(0,1,0);
	// 	glBegin(GL_POLYGON);
	// 	for(int i=0;i<nbSegments;i++)
	// 	{
	// 		Real angle = Mathr::TWO_PI/(Real)nbSegments*i;
	// 		p1 = center+sin(angle)*normalDir+cos(angle)*biNormalDir;
	// 		p2 = p1 + (p1-lightPos)*2;
	// 		glVertex3fv(p2);
	// 	}
	// 	glEnd();
}


