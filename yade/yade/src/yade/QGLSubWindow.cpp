#include "QGLSubWindow.hpp"
#include <iostream>

using namespace std;

QGLSubWindow::QGLSubWindow(QGLViewer * glViewer,int minX,int minY, int sizeX,int sizeY)
{
	this->glViewer	= glViewer;	
	this->minX	= minX;
	this->minY	= minY;
	this->sizeX	= sizeX;
	this->sizeY	= sizeY;
 	mousePress	= false;
	oldX		= 0;
	oldY		= 0;	
	selected 	= -1;	
	displayed 	= false;
		
	setBackgroundColor(77.0/255.0,238.0/255.0,234.0/255.0);
	setSelectionColor(1.0,0.0,0.0);
	setWinTranslucenty(0.6);
	setThreshold(4);	
	setMinimumWidth(-1);
	setMaximumWidth(-1);
	setMinimumHeight(-1);
	setMaximumHeight(-1);
}

QGLSubWindow::~QGLSubWindow ()
{

}

void QGLSubWindow::glDraw()
{	
	if (displayed)
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glViewer->startScreenCoordinatesSystem(false);
		glDisable(GL_LIGHTING);
		
		if (mousePress)
			drawSelectedComponent();
			
		drawWindow();
		
		glDrawInsideWindow();
		
		glViewer->stopScreenCoordinatesSystem();
		glPopAttrib();	
	}	
}

void QGLSubWindow::drawWindow()
{
	glDisable(GL_CULL_FACE);
	glAlphaFunc(GL_GREATER, 1.0f/255.0f);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);	
		
	glColor4f(color[0],color[1],color[2],winTranslucenty);
	glBegin(GL_QUADS);
		glVertex2f(minX,minY);
		glVertex2f(minX+sizeX,minY);
		glVertex2f(minX+sizeX,minY+sizeY);
		glVertex2f(minX,minY+sizeY);
	glEnd();
		
	glColor3f(color[0],color[1],color[2]);	
	glBegin(GL_LINE_LOOP);
		glVertex2f(minX-1,minY);
		glVertex2f(minX+sizeX,minY);
		glVertex2f(minX+sizeX,minY+sizeY+1);
		glVertex2f(minX-1,minY+sizeY+1);
	glEnd();
	
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_CULL_FACE);
}



void QGLSubWindow::drawSelectedComponent()
{
	glPointSize(5.0);	
	glLineWidth(2.0);
	glEnable(GL_POINT_SMOOTH);
	
	glColor3f(selectionColor[0],selectionColor[1],selectionColor[2]);
	switch (selected)
	{	
		case 0 :	glBegin(GL_POINTS);
					glVertex2f(minX,minY);
				glEnd();					
				break;
		case 1 :	glBegin(GL_POINTS);
				glVertex2f(minX+sizeX,minY);
				glEnd();					
				break;
		case 2 :	glBegin(GL_POINTS);
					glVertex2f(minX+sizeX,minY+sizeY);
				glEnd();					
				break;
		case 3 :	glBegin(GL_POINTS);
					glVertex2f(minX,minY+sizeY);
				glEnd();					
				break;
		case 4 :	glBegin(GL_LINES);
					glVertex2f(minX-1,minY);
				glVertex2f(minX+sizeX,minY);
				glEnd();
				break;
		case 5 :	glBegin(GL_LINES);
					glVertex2f(minX+sizeX,minY);
					glVertex2f(minX+sizeX,minY+sizeY+1);
				glEnd();
				break;
		case 6 :	glBegin(GL_LINES);
					glVertex2f(minX+sizeX,minY+sizeY+1);
					glVertex2f(minX-1,minY+sizeY+1);
				glEnd();
				break;
		case 7 :	glBegin(GL_LINES);
					glVertex2f(minX-1,minY+sizeY-1);
					glVertex2f(minX-1,minY);
				glEnd();
				break;
		case 8 :	glBegin(GL_LINE_LOOP);
					glVertex2f(minX-1,minY);
					glVertex2f(minX+sizeX,minY);
					glVertex2f(minX+sizeX,minY+sizeY+1);
					glVertex2f(minX-1,minY+sizeY+1);
				glEnd();
				break;
		default:	break;			
	}	
	
	glPointSize(1.0);
	glLineWidth(1.0);
	glDisable(GL_POINT_SMOOTH);
}

bool QGLSubWindow::selectedComponent(int x,int y)
{
	if (x<minX-threshold || x>minX+sizeX+threshold || y<minY-threshold || y>minY+sizeY+threshold)
		selected = -1;
	else if (abs(y-minY)<threshold && abs(x-minX)<threshold)
		selected = 0;
	else if (abs(y-minY)<threshold && abs(x-minX-sizeX)<threshold)
		selected = 1;
	else if (abs(y-minY-sizeY)<threshold && abs(x-minX-sizeX)<threshold)
		selected = 2;
	else if (abs(y-minY-sizeY)<threshold && abs(x-minX)<threshold)
		selected = 3;		
	else if (abs(y-minY)<threshold && x>minX+threshold && x<minX+sizeX-threshold)
		selected = 4;
	else if (abs(x-minX-sizeX)<threshold && y>minY+threshold && y<minY+sizeY-threshold)
		selected = 5;
	else if (abs(y-minY-sizeY)<threshold && x>minX+threshold && x<minX+sizeX-threshold)
		selected = 6;
	else if (abs(x-minX)<threshold && y>minY+threshold && y<minY+sizeY-threshold)
		selected = 7;
	else if (x>minX+threshold && x<minX+sizeX-threshold && y>minY+threshold && y<minY+sizeY-threshold)
		selected = 8;

	return (selected!=-1);

}

bool QGLSubWindow::mouseMoveEvent(QMouseEvent * e)
{
	if (mousePress)
	{
		int dx = e->x()-oldX;
		int dy = e->y()-oldY;
		switch (selected)
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
			case 8 :	minX += e->x()-oldX;
					minY += e->y()-oldY;
					break;
			default:	break;			
		}	
		if (minX<-sizeX+5) minX = -sizeX+5;
		if (minX>glViewer->width()-5) minX = glViewer->width()-5;
		if (minY<-sizeY+5) minY = -sizeY+5;
		if (minY>glViewer->height()-5) minY = glViewer->height()-5;
		if (minSizeX!=-1 && sizeX<minSizeX) sizeX=minSizeX;
		if (minSizeY!=-1 && sizeY<minSizeY) sizeY=minSizeY;
		if (maxSizeX!=-1 && sizeX>maxSizeX) sizeX=maxSizeX;
		if (maxSizeY!=-1 && sizeY>maxSizeY) sizeY=maxSizeY;
		
		oldX = e->x();
		oldY = e->y();
		glViewer->updateGL();
	}
	
	return mousePress;

}

bool QGLSubWindow::mousePressEvent(QMouseEvent *e)
{
	if (selectedComponent(e->x(),e->y()))
	{
		oldX = e->x();
		oldY = e->y();
		mousePress = true;
	}
	else
		mousePress = false;
	
	return mousePress;
}

bool QGLSubWindow::mouseReleaseEvent(QMouseEvent *)
{
	bool pressed = mousePress;
	mousePress = false;
	glViewer->updateGL();
	return pressed;
}

bool QGLSubWindow::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (selectedComponent(e->x(),e->y()))
	{
		if (selected==8)
		{
			minX = 10;
			minY = 10;
			sizeX = 100;
			sizeY = 100;
			glViewer->updateGL();
		}
		return true;
	}
	else
		return false;
}

void QGLSubWindow::swapDisplayed()
{ 
	displayed = !displayed; 
	glViewer->updateGL();
}
