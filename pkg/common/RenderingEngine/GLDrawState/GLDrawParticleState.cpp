/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawParticleState.hpp"
#include<yade/pkg-common/ParticleParameters.hpp>


#include<yade/lib-opengl/OpenGLWrapper.hpp>


void GLDrawParticleState::go(const shared_ptr<PhysicalParameters>& pp)
{
	ParticleParameters* p = static_cast<ParticleParameters*>(pp.get());
//	FIXME: so there must be a way for *not* storing the color inside those classes like BodyBoundingVolume and BodyState
//	       and simultaneously easy to set up the colors from the GUI according to some usful criterions:
//	          - to which group it belongs
//	          - to reflect some features of the drawed element
//	          - according to some certaion selection
//	          - etc...
	//glColor3v(bv->diffuseColor);
	glTranslatev(pp->se3.position);
	
	// FIXME - another problem with scaling of stuff to be drawn
	//
	//glScale(2*aabb->halfSize[0],2*aabb->halfSize[1],2*aabb->halfSize[2]);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
		glColor3(0.6,0.6,1.0);
		glVertex3(0.0,0.0,0.0);
		glVertex3v(p->velocity);
		glColor3(1.0,0.6,0.6);
		glVertex3(0.0,0.0,0.0);
		glVertex3v(p->acceleration);
	glEnd();
}

YADE_PLUGIN("GLDrawParticleState");
YADE_REQUIRE_FEATURE(OPENGL)
