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

#ifndef __GLVIEWER_H__
#define __GLVIEWER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <QGLViewer/qglviewer.h>
#include <boost/thread/mutex.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "FpsTracker.hpp"
#include "NonConnexBody.hpp"
#include "Threadable.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////



// class GLView : public QGLViewer
// {	
// 	bool needResizing;
// 	int newWidth,newHeight;
// 	public : GLView (QWidget * parent=0, QGLWidget * shareWidget=0) : QGLViewer(parent,"glview",shareWidget)
// 	{
// 		resize(300,300);
// 		setSceneCenter(0,0,0);
// 		setSceneRadius(200);
// 		showEntireScene();
// 		setAnimationPeriod(0);
// 		needResizing=true;
// 		newWidth = 300;
// 		newHeight = 300;
// 		//startAnimation();
// 		//fpsTracker = shared_ptr<FpsTracker>(new FpsTracker(this));	
// 	}
// 	
// 	public : ~GLView ()
// 	{
// 	
// 	
// 	}
// 	boost::mutex resizeMutex;
// 	public : void resizeGL( int w, int h )
// 	{
// 		
// 		boost::mutex::scoped_lock lock(resizeMutex);
// 		
// 		needResizing = true;
// 		newWidth = w;
// 		newHeight = h;
// 	}
// 
// 	public : void paintGL()
// 	{
// 		//safedraw();	
// 	}
// 	
// 	public slots: void updateGL()
// 	{       
// 		//glDraw();
// 	}	
// 	public : void safedraw()
// 	{       
// 		boost::mutex::scoped_lock lock(resizeMutex);
// 		
// 		glDraw();	
// 		preDraw();
// 		
// 		glEnable(GL_NORMALIZE);
// 		glEnable(GL_CULL_FACE);
// 	
// 		if (needResizing)
// 		{
// 			QGLWidget::resizeGL(newWidth,newHeight);
// 			glViewport( 0, 0, GLint(newWidth), GLint(newHeight) );
// 			camera()->setWindowWidthAndHeight(newWidth,newHeight);
// 			needResizing=false;
// 		}
// 		if (Omega::instance().rootBody) // if the scene is loaded
// 			Omega::instance().rootBody->glDraw();
// 		
// 		postDraw();
// 	}
// };

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class GLViewer : public QGLViewer //public Threadable<GLViewer>
{	
	// construction
	public : GLViewer (QWidget * parent=0, QGLWidget * shareWidget=0);
	public : ~GLViewer ();

 	public : void draw();

//	public : GLView *  glView;
//	public : virtual bool notEnd();
//	public : virtual void oneLoop();
	
	private : shared_ptr<FpsTracker> fpsTracker;

	protected : void mouseMoveEvent(QMouseEvent * e);
	protected : void mousePressEvent(QMouseEvent *e);
	protected : void mouseReleaseEvent(QMouseEvent *e);
	protected : void keyPressEvent(QKeyEvent *e);
	protected : void mouseDoubleClickEvent(QMouseEvent *e);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __GLVIEWER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
