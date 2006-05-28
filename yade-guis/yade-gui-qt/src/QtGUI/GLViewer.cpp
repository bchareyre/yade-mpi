/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLViewer.hpp"
#include <GL/glut.h>
#include <yade/yade-lib-threads/ThreadSynchronizer.hpp>
#include <yade/yade-lib-opengl/FpsTracker.hpp>
#include <yade/yade-core/Body.hpp>
#include <yade/yade-core/Interaction.hpp>
#include <yade/yade-core/Omega.hpp>


GLViewer::GLViewer(int id, shared_ptr<RenderingEngine> renderer, const QGLFormat& format, QWidget * parent, QGLWidget * shareWidget) : QGLViewer(format,parent,"glview",shareWidget), qglThread(this,renderer)
{
	drawGrid = false;
	viewId = id;
	setAutoBufferSwap(false);
	resize(320, 240);

	if (id==0)
		setCaption("Primary View (not closable)");
	else
		setCaption("Secondary View number "+lexical_cast<string>(id));
	show();

	wm.addWindow(shared_ptr<GLWindow>(new FpsTracker()),shared_ptr<GLWindowsManager::EventSubscription>(new GLWindowsManager::EventSubscription()));
	
	this->camera()->frame()->setWheelSensitivity(-1.0f); // reverse scrollwheel behaviour
	this->setMouseBinding(Qt::MidButton, CAMERA, TRANSLATE);
}


GLViewer::~GLViewer()
{
	
}


void GLViewer::initializeGL()
{
	QGLViewer::initializeGL();
	qglThread.initializeGL();
	glClearColor(0,0,0,0);
}


void GLViewer::finishRendering()
{
	qglThread.finish();
}


void GLViewer::joinRendering()
{
	qglThread.join();
}


void GLViewer::stopRendering()
{
	qglThread.stop();
}


void GLViewer::startRendering()
{	
	qglThread.start();
}


void GLViewer::centerScene()
{
	qglThread.centerScene();
}
	

void GLViewer::paintGL()
{
	// Handled by the GLThread.
}


void GLViewer::updateGL()
{
	// Handled by the GLThread.
}


void GLViewer::resizeEvent(QResizeEvent *evt)
{
	qglThread.resize(evt->size().width(),evt->size().height()); 
}


void GLViewer::paintEvent(QPaintEvent *)
{
	// Handled by the GLThread.
}


void GLViewer::closeEvent(QCloseEvent *)
{
	emit closeSignal(viewId);
}


void GLViewer::mouseMoveEvent(QMouseEvent * e)
{
	if (wm.mouseMoveEvent(e->x(),e->y())==-1)
		QGLViewer::mouseMoveEvent(e);
}


void GLViewer::mousePressEvent(QMouseEvent *e)
{
	if (wm.mousePressEvent(e->x(),e->y())==-1)
		QGLViewer::mousePressEvent(e);
}


void GLViewer::mouseReleaseEvent(QMouseEvent *e)
{
	if (wm.mouseReleaseEvent(e->x(),e->y())==-1)
		QGLViewer::mouseReleaseEvent(e);
}


void GLViewer::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (wm.mouseDoubleClickEvent(e->x(),e->y())==-1)
		QGLViewer::mouseDoubleClickEvent(e);
}


void GLViewer::keyPressEvent(QKeyEvent *e)
{
//	const Qt::ButtonState state = (Qt::ButtonState)(e->state() & Qt::KeyButtonMask);
	
	if ( e->key()==Qt::Key_F )
		wm.getWindow(0)->swapDisplayed(); 
	else if( e->key()==Qt::Key_G )
//		if((state != Qt::ShiftButton)
			drawGrid = !drawGrid;
//		else
//			scale = scale+1;
	else if( e->key()!=Qt::Key_Escape )
		QGLViewer::keyPressEvent(e);
}

void GLViewer::postDraw()
{
	if( drawGrid )
	{
//		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		qglColor(foregroundColor());
		glDisable(GL_LIGHTING);
		glLineWidth(0.1);
		glBegin(GL_LINES);
	
		float sizef = QGLViewer::camera()->sceneRadius()*3.0f; 
		int size = static_cast<int>(sizef);
		qglviewer::Vec v = QGLViewer::camera()->sceneCenter();
		int x = static_cast<int>(v[0]); int y = static_cast<int>(v[1]);
		float xf = (static_cast<int>(v[0]*100.0))/100.0;
		float yf = (static_cast<int>(v[1]*100.0))/100.0;
//		float nbSubdivisions = size;
//		for (int i=0; i<=nbSubdivisions; ++i)
		for (int i= -size ; i<=size; ++i )
		{
//			const float pos = size*(2.0*i/nbSubdivisions-1.0);
			glVertex2i( i   +x, -size+y);
			glVertex2i( i   +x, +size+y);
			glVertex2i(-size+x, i    +y);
			glVertex2i( size+x, i    +y);
		}
		if(sizef <= 2.0)
		{
			glColor3f(0.9,0.9,0.9);
			for (float i= -(static_cast<int>(sizef*100.0))/100.0 ; i<=sizef; i+=0.01 )
			{
				glVertex2f( i    +xf, -sizef+yf);
				glVertex2f( i    +xf, +sizef+yf);
				glVertex2f(-sizef+xf, i     +yf);
				glVertex2f( sizef+xf, i     +yf);
			}
		}
		
		glEnd();
		glPopAttrib();
		glPopMatrix();
	}
	QGLViewer::postDraw();
}

