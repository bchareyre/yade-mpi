/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "QGLThread.hpp"
#include "GLViewer.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

boost::mutex resizeMutex;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QGLThread::QGLThread(GLViewer * glv) :	Threadable<QGLThread>(), 
					needResizing(new bool(false)), 
					newWidth(new int(0)), 
					newHeight(new int(0)), 
					glViewer(glv)
{
	createThread(Omega::instance().synchronizer);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QGLThread::~QGLThread()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
	
void QGLThread::resize(int w,int h)
{
	boost::mutex::scoped_lock lock(resizeMutex);
	*newWidth = w;
	*newHeight = h;
	
	*needResizing = true;	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
	
bool QGLThread::notEnd()
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QGLThread::oneLoop()
{
	glViewer->glDraw();
	
	glViewer->makeCurrent();

	const GLfloat pos[4]	= {75.0,75.0,0.0,1.0};
	glLightfv(GL_LIGHT1, GL_POSITION, pos);
	glEnable(GL_LIGHT1);
	
	if (*needResizing)
	{
		glViewer->resizeGL(*newWidth,*newHeight);
		*needResizing=false;	
	}
	
	glViewer->preDraw();
	
	glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE);

	if (Omega::instance().rootBody) // if the scene is loaded
		Omega::instance().rootBody->glDraw();
	
	glViewer->drawLight(GL_LIGHT1);
	
	// 	fpsTracker->glDraw(); 
	// 	fpsTracker->addOneAction();

	
	glViewer->swapBuffers();
	glViewer->postDraw();
	glViewer->doneCurrent ();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

