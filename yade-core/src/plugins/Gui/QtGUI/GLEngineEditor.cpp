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

#include "GLEngineEditor.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-lib-wm3-math/Vector2.hpp>
#include <yade/yade-lib-computational-geometry/Intersections2D.hpp>
#include <yade/yade-lib-computational-geometry/Distances2D.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

GLEngineEditor::GLEngineEditor(QWidget * parent, const char * name) : QGLViewer(parent,name)
{	
	resize(800,600);

	setBackgroundColor(QColor(255,255,255));
	setMouseTracking(true);
	
	startRelation = false;
	relationSelected = false;

	relation.first = -1;
	relation.second = -1;

	updateGL();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

GLEngineEditor::~GLEngineEditor()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLEngineEditor::draw()
{			

	setBackgroundColor(QColor(255,255,255));

	drawGrid();

	startScreenCoordinatesSystem();
	glDisable(GL_DEPTH_TEST);	
	GLWindow * glw1,*glw2;
	int x1,y1,x2,y2;

	set<pair<int,int> >::iterator li = loop.begin();
	set<pair<int,int> >::iterator liEnd = loop.end();
	for(;li!=liEnd;++li)
	{
		glw1 = wm.getWindow((*li).first);
		glw2 = wm.getWindow((*li).second);

		x1 = glw1->getMinX()+glw1->getSizeX()/2;
		y1 = glw1->getMinY()+glw1->getSizeY()/2;
		x2 = glw2->getMinX()+glw2->getSizeX()/2;
		y2 = glw2->getMinY()+glw2->getSizeY()/2;
		cutSegmentWithRectangles(x1,y1,x2,y2,(*li).first,(*li).second);
		if (relationSelected && selectedRelation.first==(*li).first && selectedRelation.second==(*li).second)
			glColor3f(1.0,0.0,0.0);
		else
			glColor3f(0.0,0.0,1.0);
		drawArrow(x1,y1,x2,y2);
		
	}

	stopScreenCoordinatesSystem();

	glEnable(GL_DEPTH_TEST);

	wm.glDraw();

	startScreenCoordinatesSystem();
	glDisable(GL_DEPTH_TEST);
	if (startRelation)
	{
		glBegin(GL_LINES);
			glColor3f(1.0,0.0,0.0);
			glVertex2i(startX,startY);
			glVertex2i(endX,endY);
		glEnd();
	}


	glEnable(GL_DEPTH_TEST);
	stopScreenCoordinatesSystem();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLEngineEditor::drawArrow(int x1,int y1, int x2, int y2)
{
	glBegin(GL_LINES);
		glVertex2i(x1,y1);
		glVertex2i(x2,y2);
	glEnd();

	Vector2r v = Vector2r(x1-x2,y1-y2);
	v.normalize();
	v = v*10;
	Vector2r n = Vector2r(-v[1],v[0]);
	
	glBegin(GL_TRIANGLES);
		glVertex2i(x2,y2);
		glVertex2i(x2+2*(int)v[0]+(int)n[0],y2+2*(int)v[1]+(int)n[1]);
		glVertex2i(x2+2*(int)v[0]-(int)n[0],y2+2*(int)v[1]-(int)n[1]);
	glEnd();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLEngineEditor::drawGrid()
{
	startScreenCoordinatesSystem();
	glLineWidth(2.0); 
	glBegin(GL_LINES);
		int i;
		glColor3f(0.7,0.7,0.7);
		for(i=0;i<=width();i+=20)	
		{
			glVertex2i(i,0);
			glVertex2i(i,(int)height());
		}
		for(i=0;i<=height();i+=20)	
		{
			glVertex2i(0,i);
			glVertex2i((int)width(),i);
		}
	glEnd();
	stopScreenCoordinatesSystem();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLEngineEditor::mouseMoveEvent(QMouseEvent * e)
{
	if (startRelation)
	{
		endX = e->x();
		endY = e->y();
		int pw = wm.getPointedWindow(endX, endY);
		if (pw!=-1)
			relation.second = pw;
	}
	
	if (wm.mouseMoveEvent(e->x(),e->y())==-1)
		QGLViewer::mouseMoveEvent(e);
	updateGL();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLEngineEditor::mousePressEvent(QMouseEvent *e)
{
	if ((e->state() & Qt::KeyButtonMask)==Qt::ControlButton)
	{
		int id = wm.getPointedWindow(e->x(),e->y());
		if (id!=-1)
		{
			relation.first = id;
			relation.second = -1;			
			startRelation = true;
			GLWindow * glw = wm.getWindow(id);
			startX = glw->getMinX()+glw->getSizeX()/2;
			startY = glw->getMinY()+glw->getSizeY()/2;
			endX = e->x();
			endY = e->y();
		}
		else
		{
			relation.first = -1;
			relation.second = -1;
		}
	}
	else if ((e->state() & Qt::KeyButtonMask)==Qt::ShiftButton)
		relationSelected = selectRelation(e->x(), e->y(), 3.0, selectedRelation.first, selectedRelation.second);
	else if (wm.mousePressEvent(e->x(),e->y())==-1)
		QGLViewer::mousePressEvent(e);

	updateGL();
}


void GLEngineEditor::mouseReleaseEvent(QMouseEvent *e)
{
	if (startRelation)
	{
		startRelation = false;
		int id = wm.getPointedWindow(e->x(),e->y());
		if (id!=-1 && relation.first!=id)
		{
			relation.second = id;
			loop.insert(pair<int,int>(relation.first,relation.second));
		}
	}
	else if (wm.mouseReleaseEvent(e->x(),e->y())==-1)	
		QGLViewer::mouseReleaseEvent(e);
	updateGL();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLEngineEditor::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (wm.mouseDoubleClickEvent(e->x(),e->y())==-1)	
		QGLViewer::mouseDoubleClickEvent(e);
	updateGL();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLEngineEditor::keyPressEvent(QKeyEvent *e)
{	
	if (relationSelected && (e->key()==Qt::Key_Backspace || e->key()==Qt::Key_Delete))
	{
		int size,i;
			i=0;
			loop.erase(selectedRelation);
// 			size = data->belowFractures[selectedRelation.first].size();
// 			while (i<size && data->belowFractures[selectedRelation.first][i]!=selectedRelation.second)
// 				i++;
// 			data->belowFractures[selectedRelation.first][i] = data->belowFractures[selectedRelation.first][size-1];
// 			data->belowFractures[selectedRelation.first].resize(size-1);			
		relationSelected=false;
		updateGL();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLEngineEditor::resizeEvent(QResizeEvent * e)
{
	wm.resizeEvent(e->size().width(),e->size().height());
	QGLViewer::resizeEvent(e);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLEngineEditor::newFractureDeleted(int i)
{
	wm.deleteWindow(i);
	if (relationSelected && (selectedRelation.first==i || selectedRelation.second==i))
		relationSelected = false;
	updateGL();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLEngineEditor::updateLabel(int i)
{
	GLTextLabel * tl = (GLTextLabel*)(wm.getWindow(i));
	//tl->setText(data->fracProperties[i]->name);
	//tl->setBorderColor(data->fractures[i]->color[0],data->fractures[i]->color[1],data->fractures[i]->color[2]);
	tl->fitTextSize();
	updateGL();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLEngineEditor::cutSegmentWithRectangles(int& x1,int& y1,int& x2,int& y2, int label1, int label2)
{
	Vector2r box[4];
	GLWindow * glw;

	bool same;
	Vector2r iPoint;
	Vector2r p1 = Vector2r(x1,y1);
	Vector2r p2 = Vector2r(x2,y2);


	glw = wm.getWindow(label1);
	int x = glw->getMinX();
	int y = glw->getMinY();
	int sx = glw->getSizeX();
	int sy = glw->getSizeY();

	box[0] = Vector2r(x,y);
	box[1] = Vector2r(x+sx,y);
	box[2] = Vector2r(x+sx,y+sy);
	box[3] = Vector2r(x,y+sy);

	if (!segments2DIntersection(p1,p2,box[0],box[1],same,iPoint))
		if (!segments2DIntersection(p1,p2,box[1],box[2],same,iPoint))
			if (!segments2DIntersection(p1,p2,box[2],box[3],same,iPoint))
				segments2DIntersection(p1,p2,box[3],box[0],same,iPoint);

	x1 = (int)iPoint[0];
	y1 = (int)iPoint[1];

	glw = wm.getWindow(label2);
	x = glw->getMinX();
	y = glw->getMinY();
	sx = glw->getSizeX();
	sy = glw->getSizeY();

	box[0] = Vector2r(x,y);
	box[1] = Vector2r(x+sx,y);
	box[2] = Vector2r(x+sx,y+sy);
	box[3] = Vector2r(x,y+sy);

	if (!segments2DIntersection(p1,p2,box[0],box[1],same,iPoint))
		if (!segments2DIntersection(p1,p2,box[1],box[2],same,iPoint))
			if (!segments2DIntersection(p1,p2,box[2],box[3],same,iPoint))
				segments2DIntersection(p1,p2,box[3],box[0],same,iPoint);

	x2 = (int)iPoint[0];
	y2 = (int)iPoint[1];

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool GLEngineEditor::selectRelation(int x, int y, float threshold, int &a, int &b)
{

	GLWindow * glw1,*glw2;
	int x1,y1,x2,y2;
	float minD = threshold;
	bool selected = false;
	
	set<pair<int,int> >::iterator li = loop.begin();
	set<pair<int,int> >::iterator liEnd = loop.end();
	for(;li!=liEnd;++li)
	{
		glw1 = wm.getWindow((*li).first);
		glw2 = wm.getWindow((*li).second);

		x1 = glw1->getMinX()+glw1->getSizeX()/2;
		y1 = glw1->getMinY()+glw1->getSizeY()/2;
		x2 = glw2->getMinX()+glw2->getSizeX()/2;
		y2 = glw2->getMinY()+glw2->getSizeY()/2;
		cutSegmentWithRectangles(x1,y1,x2,y2,(*li).first,(*li).second);
		float d = fabs(distancePointSegment2D(Vector2r(x,y),Vector2r(x1,y1),Vector2r(x2,y2)));
		if (d<=minD)
		{
			minD = d;
			selected = true;
			a = (*li).first;
			b = (*li).second;
		}
	}

	return selected;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLEngineEditor::addEngine(const string& engineName )
{

	GLTextLabel * tl = new GLTextLabel();
	tl->setText(const_cast<char*>(engineName.c_str()));
	tl->setTextColor(1,0,0);
	tl->setBorderColor(1,0,1);
	tl->fitTextSize();
	tl->setResizable(false);
	tl->setWinTranslucenty(0.7);
	tl->setBorderWidth(2);

	tl->setMinX(10);
	tl->setMinY(10);	

	GLWindowsManager::EventSubscription * es = new GLWindowsManager::EventSubscription();
	es->mouseDoubleClick = false;
	wm.addWindow(tl,es);

	updateGL();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
