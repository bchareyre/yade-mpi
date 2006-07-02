/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLViewer.hpp"
#include <GL/glut.h>
#include <yade/yade-lib-opengl/FpsTracker.hpp>
#include <yade/yade-core/Body.hpp>
#include <yade/yade-core/Interaction.hpp>
#include <yade/yade-core/Omega.hpp>


GLViewer::GLViewer(int id, shared_ptr<RenderingEngine> rendererInit, const QGLFormat& format, QWidget * parent, QGLWidget * shareWidget) : QGLViewer(format,parent,"glview",shareWidget)//, qglThread(this,rendererInit)
{
	renderer=rendererInit;
	drawGrid = false;
	viewId = id;
	//setAutoBufferSwap(false);
	resize(320, 240);

	if (id==0)
		setCaption("Primary View (not closable)");
	else
		setCaption("Secondary View number "+lexical_cast<string>(id));
	show();

	wm.addWindow(shared_ptr<GLWindow>(new FpsTracker()),shared_ptr<GLWindowsManager::EventSubscription>(new GLWindowsManager::EventSubscription()));
	
	this->camera()->frame()->setWheelSensitivity(-1.0f); // reverse scrollwheel behaviour
	this->setMouseBinding(Qt::MidButton, CAMERA, TRANSLATE);

//	std::cerr << "GLViewer ctor: " << viewId << "\n";
}


GLViewer::~GLViewer()
{
//	std::cerr << "GLViewer dtor:" << viewId << "\n";
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

void GLViewer::centerScene()
{
	if (!Omega::instance().getRootBody())
		return;

	Vector3r min = Omega::instance().getRootBody()->boundingVolume->min;
	Vector3r max = Omega::instance().getRootBody()->boundingVolume->max;
	Vector3r center = (max+min)*0.5;
	Vector3r halfSize = (max-min)*0.5;
	float radius = std::max(halfSize[0] , std::max(halfSize[1] , halfSize[2]) );
	setSceneCenter(qglviewer::Vec(center[0],center[1],center[2]));
	setSceneRadius(radius*1.5);
	showEntireScene();
}

void GLViewer::draw() // FIXME maybe rename to RendererFlowControl, or something like that?
{
	if (Omega::instance().getRootBody())
	// FIXME - here we want to actually call all responsible GLDraw Actors
		renderer->render(Omega::instance().getRootBody());
	
	wm.glDraw();
	dynamic_pointer_cast<FpsTracker>(wm.getWindow(0))->addOneAction();
}

void GLViewer::postDraw()
{
	if( drawGrid ) // FIXME drawGrid is yet another RendererFlowControl's Actor.
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

void GLViewer::closeEvent(QCloseEvent *e)
{
	emit closeSignal(viewId);
}

