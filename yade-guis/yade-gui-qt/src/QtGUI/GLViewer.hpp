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
#include <yade/yade-core/RenderingEngine.hpp>
//#include "QGLThread.hpp"

class GLViewer : public QGLViewer
{	
	Q_OBJECT 
	
	friend class QGLThread;
	private :
//		QGLThread		qglThread;
		GLWindowsManager	wm;
		int			viewId;
		bool			drawGrid; // FIXME - draw grid is in fact just another GLDrawActor
		shared_ptr<RenderingEngine> renderer;

        public :
		GLViewer (int id, shared_ptr<RenderingEngine> renderer, const QGLFormat& format, QWidget * parent=0, QGLWidget * shareWidget=0);
		virtual ~GLViewer ();
		virtual void draw();
		void centerScene();


	protected :
		void keyPressEvent(QKeyEvent *e);
		void postDraw();
		void closeEvent(QCloseEvent *e);

	signals :
		virtual void closeSignal(int i);
};

#endif // GLVIEWER_HPP

