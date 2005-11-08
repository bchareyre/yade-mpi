/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "QGLThread.hpp"
#include "GLViewer.hpp"
#include <yade/yade-lib-opengl/FpsTracker.hpp>
#include <yade/yade-lib-factory/ClassFactory.hpp>


QGLThread::QGLThread(GLViewer * glv, shared_ptr<RenderingEngine> r) :	Threadable<QGLThread>(Omega::instance().getSynchronizer()),
									needCentering(new bool(false)),
									needResizing(new bool(false)),
									newWidth(new int(0)),
									newHeight(new int(0)),
									renderer(r),
									glViewer(glv)
{
	createThread();	
}


QGLThread::~QGLThread()
{
	// FIXME - why this is commented ?
	//delete needResizing;
	//delete newWidth;
	//delete newHeight;
	//delete renderer;
}


void QGLThread::initializeGL()
{
	renderer->init();
}


void QGLThread::resize(int w,int h)
{
	LOCK(*mutex);
	
	*newWidth = w;
	*newHeight = h;

	*needResizing = true;
}


void QGLThread::centerScene() 
{ 
	LOCK(*mutex);
	
	*needCentering=true;
}


bool QGLThread::notEnd()
{
	//LOCK(*mutex);
	
	return true;
}


void QGLThread::oneLoop()
{
	LOCK2(*mutex);
	LOCK(Omega::instance().getRootBodyMutex());
	
	glViewer->glDraw();

	glViewer->makeCurrent();

	if (*needResizing)
	{
		glViewer->resizeGL(*newWidth,*newHeight);
		*needResizing=false;
		glViewer->wm.resizeEvent(*newWidth,*newHeight);
	}
	
	if (*needCentering && Omega::instance().getRootBody())
	{
		Vector3r min = Omega::instance().getRootBody()->boundingVolume->min;
		Vector3r max = Omega::instance().getRootBody()->boundingVolume->max;
		Vector3r center = (max+min)*0.5;
		Vector3r halfSize = (max-min)*0.5;
		float radius = halfSize[0];
		if (halfSize[1]>radius)
			radius = halfSize[1];
		if (halfSize[2]>radius)
			radius = halfSize[2];
		
		glViewer->setSceneCenter(qglviewer::Vec(center[0],center[1],center[2]));
		glViewer->setSceneRadius(radius*1.5);
		glViewer->showEntireScene();
		*needCentering = false;
	}	
	
	glViewer->preDraw();

	if (Omega::instance().getRootBody())
		renderer->render(Omega::instance().getRootBody());
	
	glViewer->wm.glDraw();
	dynamic_pointer_cast<FpsTracker>(glViewer->wm.getWindow(0))->addOneAction();

	glViewer->postDraw();

	glViewer->swapBuffers();
	glViewer->doneCurrent ();

}


