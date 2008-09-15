// © 2008 Václav Šmilauer <eudoxos@arcig.cz>
//
// header-only utility functions for GL (moved over from extra/Shop.cpp)
//
#pragma once

#include<yade/lib-opengl/OpenGLWrapper.hpp>
#include<yade/lib-QGLViewer/qglviewer.h>
#include<sstream>
#include<iomanip>
#include<string>

struct GLUtils{

	static void GLDrawArrow(Vector3r from, Vector3r to, Vector3r color){
		glEnable(GL_LIGHTING); glColor3v(color); qglviewer::Vec a(from[0],from[1],from[2]),b(to[0],to[1],to[2]); QGLViewer::drawArrow(a,b);	
	}
	static void GLDrawLine(Vector3r from, Vector3r to, Vector3r color){
		glEnable(GL_LIGHTING); glColor3v(color);
		glBegin(GL_LINES); glVertex3v(from); glVertex3v(to); glEnd();
	}

	static void GLDrawNum(Real n, Vector3r pos, Vector3r color, unsigned precision){
		std::ostringstream oss; oss<<std::setprecision(precision)<< /* "w="<< */ (double)n;
		GLUtils::GLDrawText(oss.str(),pos,color);
	}

	static void GLDrawText(std::string txt, Vector3r pos, Vector3r color){
		glPushMatrix();
		glTranslatev(pos);
		glColor3(color[0],color[1],color[2]);
		glRasterPos2i(0,0);
		for(unsigned int i=0;i<txt.length();i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, txt[i]);
		glPopMatrix();
	}
};
