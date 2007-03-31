/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLWindow.hpp"
#include "GLWindowsManager.hpp"
#include <iostream>
#include <GL/gl.h>


GLWindow::GLWindow(int minX,int minY, int sizeX,int sizeY)
{
	this->minX			= minX;
	this->minY			= minY;
	this->sizeX			= sizeX;
	this->sizeY			= sizeY;
 	mousePress			= false;
	oldX				= 0;
	oldY				= 0;	
	selectedComponent 		= -1;	
	displayed 			= true;
	resizable			= true;

	setBackgroundColor(77.0/255.0,238.0/255.0,234.0/255.0);
	setSelectionColor(1.0,0.0,0.0);
	setBorderColor(39.0/255.0,119.0/255.0,117.0/255.0);
	setBorderWidth(2);
	setWinTranslucenty(1);
	setThreshold(4);	
	setMinimumWidth(-1);
	setMaximumWidth(-1);
	setMinimumHeight(-1);
	setMaximumHeight(-1);
	glWindowWidth=0;
	glWindowHeight=0;
}


GLWindow::~GLWindow ()
{

}


void GLWindow::resizeGlWindow(int w,int h)
{
	glWindowWidth=w;
	glWindowHeight=h;
}


void GLWindow::glDraw()
{	
	if (displayed)
	{				
		glDisable(GL_CULL_FACE);	
		glDisable(GL_DEPTH_TEST);

		drawWindow();
		
		drawSelectedComponent();
	
		glDrawInsideWindow();	
	
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}	
}


void GLWindow::drawWindow()
{
	if (winTranslucenty!=1)
	{
		glAlphaFunc(GL_GREATER, 1.0f/255.0f);
		glEnable(GL_ALPHA_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);	
	}
		
	glColor4f(color[0],color[1],color[2],winTranslucenty);
	glBegin(GL_QUADS);
		glVertex2f(minX,minY);
		glVertex2f(minX+sizeX,minY);
		glVertex2f(minX+sizeX,minY+sizeY);
		glVertex2f(minX,minY+sizeY);
	glEnd();
	
	glLineWidth(borderWidth);
	glColor3f(borderColor[0],borderColor[1],borderColor[2]);	
	glBegin(GL_LINE_LOOP);
		glVertex2f(minX-1,minY);
		glVertex2f(minX+sizeX,minY);
		glVertex2f(minX+sizeX,minY+sizeY+1);
		glVertex2f(minX-1,minY+sizeY+1);
	glEnd();
		
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glLineWidth(1.0);
}


void GLWindow::drawSelectedComponent()
{	
	glLineWidth(2.0);
	
	glColor3f(selectionColor[0],selectionColor[1],selectionColor[2]);
	int p1x,p1y;
	switch (selectedComponent)
	{	
		case 0 :		
				glBegin(GL_LINES);
					p1x = minX-5;
					p1y = minY-5;
					glVertex2f(p1x,p1y);
					glVertex2f(p1x,p1y+10);
					glVertex2f(p1x,p1y);
					glVertex2f(p1x+10,p1y);
					p1x = minX-10;
					p1y = minY-10;
					glVertex2f(p1x,p1y);
					glVertex2f(p1x,p1y+15);
					glVertex2f(p1x,p1y);
					glVertex2f(p1x+15,p1y);
				glEnd();
				break;
		case 1 :
				glBegin(GL_LINES);
					p1x = minX+sizeX+5;
					p1y = minY-5;
					glVertex2f(p1x,p1y);
					glVertex2f(p1x,p1y+10);
					glVertex2f(p1x,p1y);
					glVertex2f(p1x-10,p1y);
					p1x = minX+sizeX+10;
					p1y = minY-10;
					glVertex2f(p1x,p1y);
					glVertex2f(p1x,p1y+15);
					glVertex2f(p1x,p1y);
					glVertex2f(p1x-15,p1y);
				glEnd();					
				break;
		case 2 :
				glBegin(GL_LINES);
					p1x = minX+sizeX+5;
					p1y = minY+sizeY+5;
					glVertex2f(p1x,p1y);
					glVertex2f(p1x,p1y-10);
					glVertex2f(p1x,p1y);
					glVertex2f(p1x-10,p1y);
					p1x = minX+sizeX+10;
					p1y = minY+sizeY+10;
					glVertex2f(p1x,p1y);
					glVertex2f(p1x,p1y-15);
					glVertex2f(p1x,p1y);
					glVertex2f(p1x-15,p1y);
				glEnd();					
				break;
		case 3 :
				glBegin(GL_LINES);
					p1x = minX-5;
					p1y = minY+sizeY+5;
					glVertex2f(p1x,p1y);
					glVertex2f(p1x,p1y-10);
					glVertex2f(p1x,p1y);
					glVertex2f(p1x+10,p1y);
					p1x = minX-10;
					p1y = minY+sizeY+10;
					glVertex2f(p1x,p1y);
					glVertex2f(p1x,p1y-15);
					glVertex2f(p1x,p1y);
					glVertex2f(p1x+15,p1y);
				glEnd();					
				break;
		case 4 :
				glBegin(GL_LINES);
				glVertex2f(minX-1,minY);
				glVertex2f(minX+sizeX,minY);
				glEnd();
				break;
		case 5 :
				glBegin(GL_LINES);
				glVertex2f(minX+sizeX,minY);
				glVertex2f(minX+sizeX,minY+sizeY+1);
				glEnd();
				break;
		case 6 :
				glBegin(GL_LINES);
				glVertex2f(minX+sizeX,minY+sizeY+1);
				glVertex2f(minX-1,minY+sizeY+1);
				glEnd();
				break;
		case 7 :
				glBegin(GL_LINES);
				glVertex2f(minX-1,minY+sizeY-1);
				glVertex2f(minX-1,minY);
				glEnd();
				break;
		case 8 :
				glBegin(GL_LINE_LOOP);
				glVertex2f(minX-1,minY);
				glVertex2f(minX+sizeX,minY);
				glVertex2f(minX+sizeX,minY+sizeY+1);
				glVertex2f(minX-1,minY+sizeY+1);
				glEnd();
				break;
		default:	break;			
	}	
	
	glLineWidth(1.0);
}


bool GLWindow::selectComponent(int x,int y)
{
	if (resizable)
	{
		if (x<minX-threshold || x>minX+sizeX+threshold || y<minY-threshold || y>minY+sizeY+threshold )
			selectedComponent = -1;
		else if (abs(y-minY)<threshold && abs(x-minX)<threshold)
			selectedComponent = 0;
		else if (abs(y-minY)<threshold && abs(x-minX-sizeX)<threshold)
			selectedComponent = 1;
		else if (abs(y-minY-sizeY)<threshold && abs(x-minX-sizeX)<threshold)
			selectedComponent = 2;
		else if (abs(y-minY-sizeY)<threshold && abs(x-minX)<threshold)
			selectedComponent = 3;		
		else if (abs(y-minY)<threshold && x>minX+threshold && x<minX+sizeX-threshold)
			selectedComponent = 4;
		else if (abs(x-minX-sizeX)<threshold && y>minY+threshold && y<minY+sizeY-threshold)
			selectedComponent = 5;
		else if (abs(y-minY-sizeY)<threshold && x>minX+threshold && x<minX+sizeX-threshold)
			selectedComponent = 6;
		else if (abs(x-minX)<threshold && y>minY+threshold && y<minY+sizeY-threshold)
			selectedComponent = 7;
		else if (x>minX+threshold && x<minX+sizeX-threshold && y>minY+threshold && y<minY+sizeY-threshold)
			selectedComponent = 8;
	}
	else
	{
		if (x>minX+threshold && x<minX+sizeX-threshold && y>minY+threshold && y<minY+sizeY-threshold)
			selectedComponent = 8;
		else
			selectedComponent = -1;
	}

	return (selectedComponent!=-1);

}


bool GLWindow::mouseMoveEvent(int x, int y)
{
	if (mousePress)
	{
		int dx = x-oldX;
		int dy = y-oldY;
		switch (selectedComponent)
		{
			case 0 :	minX += dx;
					minY += dy;
					sizeX -= dx;
					sizeY -= dy;
					break;
			case 1 :	minY += dy;
					sizeX += dx;
					sizeY -= dy;
					break;
			case 2 :	sizeX += dx;
					sizeY += dy;
					break;
			case 3 :	minX += dx;
					sizeX -= dx;
					sizeY += dy;
					break;
			case 4 :	minY += dy;
					sizeY -= dy;
					break;
			case 5 :	sizeX += dx;
					break;
			case 6 :	sizeY += dy;
					break;
			case 7 :	minX += dx;
					sizeX -= dx;
					break;
			case 8 :	minX += x-oldX;
					minY += y-oldY;
					break;
			default:	break;			
		}	

		if (minX<5) 
			minX = 5;
		if (minX+sizeX>glWindowWidth-5) 
			minX = glWindowWidth-5-sizeX;

		if (minY<5) 
			minY = 5;
		if (minY+sizeY>glWindowHeight-5) 
			minY = glWindowHeight-5-sizeY;


		if (minSizeX!=-1 && sizeX<minSizeX) 
			sizeX=minSizeX;
		if (minSizeY!=-1 && sizeY<minSizeY) 
			sizeY=minSizeY;

		if (maxSizeX!=-1 && sizeX>maxSizeX) 
			sizeX=maxSizeX;
		if (maxSizeY!=-1 && sizeY>maxSizeY) 
			sizeY=maxSizeY;
		
		oldX = x;
		oldY = y;
  	}
	
	return mousePress;

}


bool GLWindow::mousePressEvent(int x, int y)
{
	if (selectComponent(x,y))
	{
		oldX = x;
		oldY = y;
		mousePress = true;
	}
	else
		mousePress = false;
	
	return mousePress;
}


bool GLWindow::mouseReleaseEvent(int , int )
{
	bool pressed = mousePress;
	selectedComponent = -1;
	mousePress = false;
	return pressed;
}


bool GLWindow::mouseDoubleClickEvent(int x, int y) 
{
	if (selectComponent(x,y))
	{
		if (selectedComponent==8)
		{
			minX = 10;
			minY = 10;
			sizeX = 100;
			sizeY = 100;
		}
		selectedComponent = -1;
		mousePress = false;
		return true;
	}
	else
	{
		selectedComponent = -1;
		mousePress = false;
		return false;
	}
		
}


void GLWindow::swapDisplayed()
{ 
	displayed = !displayed;
}


bool GLWindow::mouseIsOnWindows(int x,int y)
{
	return (x>minX && x<minX+sizeX && y>minY && y<minY+sizeY);
}

