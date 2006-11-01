/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi, Janek Kozicki                  *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLVIEWER_HPP
#define GLVIEWER_HPP

#include <QGLViewer/qglviewer.h>
//#include <yade/yade-lib-opengl/GLWindowsManager.hpp> // remove that...
#include <yade/yade-core/RenderingEngine.hpp>

class GLViewer : public QGLViewer
{	
	Q_OBJECT 
	
	friend class QGLThread;
	private :
//		GLWindowsManager	wm;
		int			viewId;
		bool			drawGrid; // FIXME - draw grid is in fact just another GLDrawActor
		bool			isMoving;
		bool			wasDynamic;
		shared_ptr<RenderingEngine> renderer;

        public :
		GLViewer (int id, shared_ptr<RenderingEngine> renderer, const QGLFormat& format, QWidget * parent=0, QGLWidget * shareWidget=0);
		virtual ~GLViewer ();
		virtual void draw();
		virtual void drawWithNames();
		void centerScene();
		void notMoving();


	protected :
		virtual void keyPressEvent(QKeyEvent *e);
		virtual void postDraw();
		virtual void closeEvent(QCloseEvent *e);
		virtual void postSelection(const QPoint& point);
		virtual void endSelection(const QPoint &point);

	signals :
		virtual void closeSignal(int i);
};

#endif // GLVIEWER_HPP

