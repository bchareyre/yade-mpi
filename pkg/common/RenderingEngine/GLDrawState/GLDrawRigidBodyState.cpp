/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawRigidBodyState.hpp"
#include "GLDrawParticleState.hpp"
#include "RigidBodyParameters.hpp"


#include<yade/lib-opengl/OpenGLWrapper.hpp>


void GLDrawRigidBodyState::go(const shared_ptr<PhysicalParameters>& bst)
{
	// drawing RigidBodyState involves drawing ParticleState too
	// FIXME - is this a correct place for doing that?
	glPushMatrix();
	GLDrawParticleState g;
	g.go(bst);
	glPopMatrix();

	RigidBodyParameters * rb = static_cast<RigidBodyParameters*>(bst.get());
//	FIXME: so there must be a way for *not* storing the color inside those classes like BodyBoundingVolume and BodyState
//	       and simultaneously easy to set up the colors from the GUI according to some usful criterions:
//	          - to which group it belongs
//	          - to reflect some features of the drawed element
//	          - according to some certaion selection
//	          - etc...
//	glColor3v(bst->diffuseColor);
	glTranslatev(bst->se3.position);

	// FIXME - another problem with scaling of stuff to be drawn
	//
	//glScale(2*aabb->halfSize[0],2*aabb->halfSize[1],2*aabb->halfSize[2]);
	
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
		glColor3(0.0,0.0,1.0);
		glVertex3(0.0,0.0,0.0);
		glVertex3v(rb->angularVelocity);
		glColor3(1.0,0.0,0.0);
		glVertex3(0.0,0.0,0.0);
		glVertex3v(rb->angularAcceleration);
	glEnd();
}

