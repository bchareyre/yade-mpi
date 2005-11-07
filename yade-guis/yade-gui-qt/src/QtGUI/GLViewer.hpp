/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLVIEWER_HPP
#define GLVIEWER_HPP

#include <QGLViewer/qglviewer.h>
#include <yade/yade-lib-opengl/GLWindowsManager.hpp>
#include "QGLThread.hpp"

class GLViewer : public QGLViewer
{	
	Q_OBJECT 
	
	friend class QGLThread;
	private :
		QGLThread		qglThread;
		GLWindowsManager	wm;
		int			viewId;
	
	public :
		GLViewer (int id, shared_ptr<RenderingEngine> renderer, const QGLFormat& format, QWidget * parent=0, QGLWidget * shareWidget=0);
		virtual ~GLViewer ();

		void centerScene();
		void finishRendering();
		void joinRendering();
		void stopRendering();
		void startRendering();
		void paintGL();	
		void initializeGL(); 

	protected :
		void resizeEvent(QResizeEvent *evt);
		void paintEvent(QPaintEvent *);
		void closeEvent(QCloseEvent *evt);
		void mouseMoveEvent(QMouseEvent * e);
		void mousePressEvent(QMouseEvent *e);
		void mouseReleaseEvent(QMouseEvent *e);
		void keyPressEvent(QKeyEvent *e);
		void mouseDoubleClickEvent(QMouseEvent *e);
	
	public slots :
		void updateGL();
	signals :
		virtual void closeSignal(int i);
};

#endif // GLVIEWER_HPP

