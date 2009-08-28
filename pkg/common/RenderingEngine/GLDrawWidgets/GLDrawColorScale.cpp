/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawColorScale.hpp"

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <yade/pkg-common/ColorScale.hpp>
#include<yade/lib-opengl/OpenGLWrapper.hpp>

void GLDrawColorScale::go(const shared_ptr<GeometricalModel>& gm, const shared_ptr<PhysicalParameters>& ph,bool wire)
{
    ColorScale* cs = static_cast<ColorScale*>(gm.get());
    
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	//int WinH = glutGet(GLUT_WINDOW_HEIGHT);
	//int WinW = glutGet(GLUT_WINDOW_WIDTH);
	//gluOrtho2D(0,WinW, 0,WinH);
	gluOrtho2D(0.,1.,0.,1.);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_LIGHTING);

	const Real oX = cs->posX;
	const Real oY = cs->posY;
	const Real w  = cs->width;
	const vector<Vector3r>& colors = cs->colors;
	const Real nbSteps = colors.size();
	const Real hStep = cs->height/(nbSteps-1);
	glBegin(GL_QUAD_STRIP);
		for (int i=0; i<nbSteps; ++i)
		{
		  glColor3f(colors[i][0],colors[i][1],colors[i][2]);
		  glVertex2f(oX, oY+i*hStep);
		  glVertex2f(oX+w,oY+i*hStep);
		}
	glEnd();

	const vector<string>& labels = cs->labels;
	const int nbLabels = labels.size();
	const Real lStep = cs->height/(nbLabels-1);
	for (int i=0; i<nbLabels; ++i)
	{
		glColor3f(1.,1.,1.);
		glRasterPos2f(oX+w,oY+i*lStep);
		for(unsigned int j=0;j<labels[i].length();j++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, labels[i][j]);
	}

	glColor3f(1.,1.,1.);
	glRasterPos2f(oX,oY+(nbSteps-1)*hStep+0.05);
	for(unsigned int i=0;i<cs->title.length();i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, cs->title[i]);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);	

}
YADE_PLUGIN((GLDrawColorScale));
YADE_REQUIRE_FEATURE(OPENGL)
