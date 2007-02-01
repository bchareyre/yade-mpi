/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@mail.berlios.de                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawClosestFeatures.hpp"
#include "ClosestFeatures.hpp"


#include <yade/yade-lib-opengl/OpenGLWrapper.hpp>


void GLDrawClosestFeatures::go(
		const shared_ptr<InteractionGeometry>& ig,
		const shared_ptr<Interaction>&,
		const shared_ptr<Body>& b1,
		const shared_ptr<Body>& b2,
		bool wireFrame)
{
	ClosestFeatures* p = static_cast<ClosestFeatures*>(ig.get());
//	FIXME: so there must be a way for *not* storing the color inside those classes like BodyBoundingVolume and BodyState
//	       and simultaneously easy to set up the colors from the GUI according to some usful criterions:
//	          - to which group it belongs
//	          - to reflect some features of the drawed element
//	          - according to some certaion selection
//	          - etc...
	//glColor3v(bv->diffuseColor);
	glTranslatev(p->closestsPoints[0].first);
	
	// FIXME - another problem with scaling of stuff to be drawn
	//
	//glScale(2*aabb->halfSize[0],2*aabb->halfSize[1],2*aabb->halfSize[2]);
	glDisable(GL_LIGHTING);

	// too lazy to write a loop....
	glBegin(GL_LINES);
		glColor3(1.0,0.0,0.0);
		glVertex3(0.0,0.0,0.0);
		glVertex3v(p->closestsPoints[0].second-p->closestsPoints[0].first);
	glEnd();
}

