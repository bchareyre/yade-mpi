/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLTextLabel.hpp"

#include <GL/gl.h>
#include <GL/glut.h>
#include <string.h>


GLTextLabel::GLTextLabel (int minX,int minY, int sizeX,int sizeY,char * text) : GLWindow(minX,minY,sizeX,sizeY)
{
	setText(text);

	setTextColor(1,0,0);
}


GLTextLabel::~GLTextLabel ()
{

}


void GLTextLabel::glDrawInsideWindow()
{	
	drawString(text,10,10,textColor);
}


void GLTextLabel::drawString(char * str,int ,int ,Vector3r c)
{
	if (str!=NULL)		
	{
		glColor3f(c[0],c[1],c[2]);
		glPushMatrix();
		glRasterPos2i(minX+9,minY+sizeY/2+5);
		for(int i=0;str[i]!='\0';i++)
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);
		glPopMatrix();
	}
}


void GLTextLabel::setText(char * text)
{
	if (this->text==NULL)
			delete this->text;

	if (text!=NULL)
	{
		int size = 0;
		while (text[size]!='\0')
			size++;

		this->text = new char[size];

		strcpy(this->text,text);
	}
	else
	{
		this->text = new char[2];
		strcpy(this->text," ");
	}
}


void GLTextLabel::fitTextSize()
{
	int size = 0;
	while (text[size]!='\0')
		size++;

	size+=2;
	sizeX = 9*size;
	sizeY = 15*3;
}


void GLTextLabel::setTextColor(float r, float g,float b)
{
	textColor = Vector3r(r,g,b);
}

