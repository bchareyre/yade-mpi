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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade-lib-opengl/GLWindowsManager.hpp>
#include "QGLThread.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class GLViewer : public QGLViewer
{	
	Q_OBJECT 
	
	private : GLWindowsManager wm;
	private : int viewId;
	
	// construction
	public : GLViewer (int id, shared_ptr<RenderingEngine> renderer, const QGLFormat& format, QWidget * parent=0, QGLWidget * shareWidget=0);
	public : ~GLViewer ();

	private : QGLThread qglThread;
	friend class QGLThread;
	
	protected : void resizeEvent(QResizeEvent *evt);
	protected : void paintEvent(QPaintEvent *);
	protected : void closeEvent(QCloseEvent *evt);
	
	public    : void centerScene();
	public    : void finishRendering();
	public    : void joinRendering();
	public    : void stopRendering();
	public    : void startRendering();
	
	signals : virtual void closeSignal(int i);
	
	public : void paintGL();	
	public slots: void updateGL();
	public : void initializeGL(); 
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
