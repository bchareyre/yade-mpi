/*************************************************************************
*  Copyright (C) 2007 by Janek Kozicki                                   *
*  cosurgi@mail.berlios.de                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawElasticContactInteraction.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-common/SimpleElasticInteraction.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>

#include<yade/lib-opengl/OpenGLWrapper.hpp>
#include<yade/lib-QGLViewer/qglviewer.h>

GLDrawElasticContactInteraction::GLDrawElasticContactInteraction() : boxIndex(-1), maxLength(0.0000001)
{
}

void GLDrawElasticContactInteraction::drawArrow(const Vector3r from,const Vector3r to,const Vector3r color)
{
	glEnable(GL_LIGHTING);
	glColor3v(color);
	qglviewer::Vec a(from[0],from[1],from[2]),b(to[0],to[1],to[2]);
	QGLViewer::drawArrow(a,b);	
};

void GLDrawElasticContactInteraction::drawFlatText(const Vector3r pos,const std::string txt)
{
	glPushMatrix();
	glTranslatev(pos);
	glColor3(1.0,1.0,0.0);
	glRasterPos2i(0,0);
	for(unsigned int i=0;i<txt.length();i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, txt[i]);
	glPopMatrix();
};

void GLDrawElasticContactInteraction::go(
		const shared_ptr<InteractionPhysics>& ih,
		const shared_ptr<Interaction>& i,
		const shared_ptr<Body>& b1,
		const shared_ptr<Body>& b2,
		bool wireFrame)
{
//	if(!i->isReal) return;

	ElasticContactInteraction*    ph = static_cast<ElasticContactInteraction*>(ih.get());
	SpheresContactGeometry*    sc = static_cast<SpheresContactGeometry*>(i->interactionGeometry.get());
//	Vector3r pos1   = b1->physicalParameters->se3.position;
//	Vector3r pos2   = b2->physicalParameters->se3.position;

	/// skip drawing interaction with boxes....
//		if(boxIndex == -1 && b1->geometricalModel->getClassName() == "Box") boxIndex = b1->geometricalModel->getClassIndex();
//		if(boxIndex == -1 && b2->geometricalModel->getClassName() == "Box") boxIndex = b2->geometricalModel->getClassIndex();
//		if(b1->geometricalModel->getClassIndex() == boxIndex || b2->geometricalModel->getClassIndex() == boxIndex) return;
	//

	Vector3r cp = sc->contactPoint;
	Vector3r normal = sc->normal;
//	Vector3r middle = 0.5*(pos1+pos2);
//	Vector3r dist   = 0.5*(pos2-pos1);
//	Vector3r pos1d  = 0.9*pos1 + 0.1*pos2;
//	Vector3r pos2d  = 0.1*pos1 + 0.9*pos2;
//	Real     size   = dist.Length()*0.5;
	Real     size   = 0.5*(sc->radius1 + sc->radius2);

// draw connecting line.
	glBegin(GL_LINES);
	glColor3(1.4,1.4,0.4);
//	glVertex3v(pos1);
//	glVertex3v(pos2);
	glVertex3v(cp-normal*sc->radius1);
	glVertex3v(cp+normal*sc->radius2);
	glEnd();

// draw normal
	drawArrow(cp, cp+normal*size*0.9 ,Vector3r(0,i->isReal?1:0.4,0));
// draw prevNormal
//	drawArrow(middle, middle+ph->prevNormal*size*0.9 ,Vector3r(i->isReal?1:0.4,0,0));
// draw shearForce
	maxLength = std::max(maxLength,ph->shearForce.Length());
	if(wireFrame) maxLength = 0.0000001;
	drawArrow(cp, cp+ph->shearForce*size*10.0/maxLength ,Vector3r(0,0,i->isReal?1:0.4));

// write A,B
//	drawFlatText(pos1d,std::string("  A ") + boost::lexical_cast<std::string>(b1->getId()));
//	drawFlatText(pos2d,std::string("  B ") + boost::lexical_cast<std::string>(b2->getId()));

}

YADE_PLUGIN();
