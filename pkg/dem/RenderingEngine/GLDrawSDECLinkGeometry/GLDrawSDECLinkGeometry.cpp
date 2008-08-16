/*************************************************************************
*  Copyright (C) 2007 by Janek Kozicki                                   *
*  cosurgi@mail.berlios.de                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"GLDrawSDECLinkGeometry.hpp"
#include<yade/pkg-dem/SDECLinkGeometry.hpp>
#include<yade/pkg-common/NormalShearInteractions.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>

#include<yade/lib-opengl/OpenGLWrapper.hpp>

GLDrawSDECLinkGeometry::GLDrawSDECLinkGeometry() : midMax(0), forceMax(0)
{
}

void GLDrawSDECLinkGeometry::go(
		const shared_ptr<InteractionGeometry>& ig,
		const shared_ptr<Interaction>& ip,
		const shared_ptr<Body>& b1,
		const shared_ptr<Body>& b2,
		bool wireFrame)
{
	SDECLinkGeometry*	sc = static_cast<SDECLinkGeometry*>(ig.get());
	SDECLinkPhysics*	el = static_cast<SDECLinkPhysics*>(ip->interactionPhysics.get());

	Vector3r contactPoint = (b1->physicalParameters->se3.position + b2->physicalParameters->se3.position)*0.5 ;

	if(wireFrame)
	{
		midMax=0;
		forceMax=0;

		glTranslatev( contactPoint );
		glBegin(GL_LINES);
			glColor3(0.2,0.2,0.9);
			glVertex3(0.0,0.0,0.0);
			glVertex3v(-1.0*(sc->normal*sc->radius1));
			glVertex3(0.0,0.0,0.0);
			glVertex3v( 1.0*(sc->normal*sc->radius2));
		glEnd();
	}
	else
	{
		Quaternionr q;
		q.Align(Vector3r(1.0,0.0,0.0),sc->normal);
		Vector3r axis;	
		Real angle;
		q.ToAxisAngle(axis,angle);

		Real mid = (sc->radius1 + sc->radius2);
		Real dif = (sc->radius1 - sc->radius2)*0.5;
		midMax = std::max(mid,midMax);

		glTranslatev( contactPoint-(sc->normal*dif) );
		glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);
		
	// FIXME - we need a way to give parameters from outside, again.... so curerntly this scale is hardcoded here
		if( (!ip->isNew) && /*ip->isReal &&*/ ip->interactionPhysics)
		{
			Real force = el->normalForce.Length()/600;
			forceMax = std::max(force,forceMax);

			Real scale = midMax*(force/forceMax)*0.3;
			glScalef(mid,scale,scale);
		}
		else
			glScalef(mid,mid*0.05,mid*0.05);
		if( sc->normal.Dot(el->normalForce) < 0 )
			glColor3(0.5,0.5,0.9);
		else
			glColor3(0.9,0.5,0.5);
	// ///////////

		glEnable(GL_LIGHTING);
		glutSolidCube(1.0);

	}
}

YADE_PLUGIN();
