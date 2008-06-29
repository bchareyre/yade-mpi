/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi, Janek Kozicki                  *
*  2008 Václav Šmilauer
*  *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once

#include<yade/core/Omega.hpp>
#include<yade/pkg-common/OpenGLRenderingEngine.hpp>
#include<yade/lib-QGLViewer/qglviewer.h>
#include<yade/lib-QGLViewer/constraint.h>
#include<set>

/* There can be several clipping planes (default is up to 2).
 * Clipping plane is manipulated after hitting F1, F2, .... To end manipulation, press Escape.
 * During manipulation, hitting 1,2,... will align the current plane with #1,2,... (same orientation).
 * Hitting 'R' will reverse the plane (normal*=-1).
 * Alt-1,Alt-2,... adds/removes the respective plane to boud group: when any of planes in the group is moved,
 * 	all other planes are moved as well so that their relative positions are the same.
 *
 */

class GLViewer : public QGLViewer
{	
	Q_OBJECT 
	
	friend class QGLThread;
	private :
//		GLWindowsManager	wm;
		bool 			drawGridXYZ[3];
		bool			isMoving;
		bool			wasDynamic;
		float			cut_plane;
		int			cut_plane_delta;
		shared_ptr<OpenGLRenderingEngine> renderer;
		int manipulatedClipPlane;
		set<int> boundClipPlanes;
		shared_ptr<qglviewer::LocalConstraint> xyPlaneConstraint;
		string strBoundGroup(){string ret;FOREACH(int i, boundClipPlanes) ret+=" "+lexical_cast<string>(i+1);return ret;}

        public :
		GLViewer (int id, shared_ptr<OpenGLRenderingEngine> renderer, const QGLFormat& format, QWidget * parent=0, QGLWidget * shareWidget=0);
		virtual ~GLViewer (){};
		virtual void draw();
		virtual void drawWithNames();
		void centerScene();
		void mouseMovesCamera();
		void mouseMovesManipulatedFrame(qglviewer::Constraint* c=NULL);
		void resetManipulation();
		void startClipPlaneManipulation(int planeNo);
		int viewId;


		DECLARE_LOGGER;
	protected :
		virtual void keyPressEvent(QKeyEvent *e);
		virtual void postDraw();
		virtual void closeEvent(QCloseEvent *e);
		virtual void postSelection(const QPoint& point);
		virtual void endSelection(const QPoint &point);
};
