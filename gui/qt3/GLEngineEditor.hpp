/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GL_ENGINE_EDITOR_HPP
#define GL_ENGINE_EDITOR_HPP

#include <QGLViewer/qglviewer.h>

#include <set>

#include<yade/lib-opengl/GLWindowsManager.hpp>
#include<yade/lib-opengl/GLTextLabel.hpp>

using namespace std;

class GLEngineEditor : public QGLViewer
{
	Q_OBJECT

	private :
		struct lessThanPair
		{
			bool operator()(const pair<int,int>& p1, const pair<int,int>& p2)
			{
				return (p1.first<p2.first || p1.first==p2.first && p1.second<p2.second);
			}
		};
				
		set<pair<int,int>, lessThanPair > loop;

		GLWindowsManager wm;

		bool startRelation;
		bool relationSelected;
		pair<int,int> selectedRelation;
		pair<int,int> relation;
		int startX;
		int startY;
		int endX;
		int endY;
		int selectedEngine;
		Vector3r savedColor;
		int firstEngine;

		void drawGrid();
		void drawArrow(int x1,int y1, int x2, int y2);
		void cutSegmentWithRectangles(int& x1,int& y1,int& x2,int& y2, int label1, int label2);
		bool selectRelation(int x, int y, float threshold, int &a, int &b);
		int selectEngine(int x, int y);

		int createNewTextLabel(const string& name, float r, float g, float b );

	public :
		GLEngineEditor(QWidget * parent=0, const char * name=0);
		~GLEngineEditor();
		
		void draw();

		void updateLabel(int i);
		int getFirstEngine() { return firstEngine;};
		
		int getWindowMinX(int i) { return wm.getWindow(i)->getMinX(); };
		int getWindowMinY(int i) { return wm.getWindow(i)->getMinY(); };

		int addEngine(const string& engineName );
		int addDeusExMachina	(const string& engineName );
		int addMetaDispatchingEngine2D(const string& engineName, const string& engineUnitName, const string& baseClass1Name,  const string& baseClass2Name);
		int addMetaDispatchingEngine1D(const string& engineName, const string& engineUnitName, const string& baseClass1Name);

		bool verify(string& errorMessage);
		int findRelationStartingWith(int first);
	
	protected :
		void mouseMoveEvent(QMouseEvent * e);
		void mousePressEvent(QMouseEvent *e);
		void mouseReleaseEvent(QMouseEvent *e);
		void mouseDoubleClickEvent(QMouseEvent *e);

		void keyPressEvent(QKeyEvent *e);
		void resizeEvent ( QResizeEvent * e);

	signals :
		void verifyValidity();
		void engineSelected(int);
		void deleteEngine(int);

};

#endif //  GL_ENGINE_EDITOR_HPP

