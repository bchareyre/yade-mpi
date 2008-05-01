//  Copyright (C) 2004 by Olivier Galizzi, Janek Kozicki
//  qt4 port 2008 Václav Šmilauer <eudoxos@arcig.cz>

#pragma once

#include<yade/core/Omega.hpp>
#include<yade/core/RenderingEngine.hpp>
#include<QGLViewer/qglviewer.h>

class GLViewer4 : public QGLViewer
{
	//Q_OBJECT;
	//friend class QGLThread;
		
	private:
		int			viewId;
		bool			drawGrid; // FIXME - draw grid is in fact just another GLDrawActor
		bool			isMoving;
		bool			wasDynamic;
		shared_ptr<RenderingEngine> renderer;

	public :
		GLViewer4 (int id, shared_ptr<RenderingEngine> renderer, const QGLFormat& format, QWidget * parent=0, QGLWidget * shareWidget=0);
		virtual ~GLViewer4() {};
		virtual void draw();
		//virtual void init(){};
		//virtual QString helpString() const {return QString("@@@");}
		virtual void drawWithNames();
		void centerScene();
		void notMoving();
	protected :
		virtual void keyPressEvent(QKeyEvent *e);
		virtual void postDraw();
		//virtual void closeEvent(QCloseEvent *e);
		virtual void postSelection(const QPoint& point);
		virtual void endSelection(const QPoint &point);
	//slot:
	//		void callUpdateGL(){cerr<<"@"; updateGL();}

	signals :
//		virtual void closeSignal(int i);
};


