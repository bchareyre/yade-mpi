/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLEngineEditor.hpp"
#include <Wm3Vector2.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/lib-computational-geometry/Intersections2D.hpp>
#include<yade/lib-computational-geometry/Distances2D.hpp>


GLEngineEditor::GLEngineEditor(QWidget * parent, const char * name) : QGLViewer(parent,name)
{	
	resize(800,600);

	setBackgroundColor(QColor(255,255,255));
	setMouseTracking(true);
	
	startRelation = false;
	relationSelected = false;

	relation.first = -1;
	relation.second = -1;
	selectedEngine = -1;
	firstEngine = -1;

	updateGL();
}


GLEngineEditor::~GLEngineEditor()
{

}


void GLEngineEditor::draw()
{			

	setBackgroundColor(QColor(255,255,255));

	drawGrid();

	startScreenCoordinatesSystem();
	glDisable(GL_DEPTH_TEST);	
	shared_ptr<GLWindow> glw1,glw2;
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

	if (selectedEngine!=-1)
	{
		glw1 = wm.getWindow(selectedEngine);
		glBegin(GL_LINE_LOOP);
			glColor3f(1.0,0.0,0.0);
			glVertex2i(glw1->getMinX()-4,glw1->getMinY()-3);
			glVertex2i(glw1->getMinX()+glw1->getSizeX()+3,glw1->getMinY()-3);
			glVertex2i(glw1->getMinX()+glw1->getSizeX()+3,glw1->getMinY()+glw1->getSizeY()+4);
			glVertex2i(glw1->getMinX()-4,glw1->getMinY()+glw1->getSizeY()+4);
		glEnd();
	}		

	glEnable(GL_DEPTH_TEST);
	stopScreenCoordinatesSystem();

}


void GLEngineEditor::drawArrow(int x1,int y1, int x2, int y2)
{
	glBegin(GL_LINES);
		glVertex2i(x1,y1);
		glVertex2i(x2,y2);
	glEnd();

	Vector2r v = Vector2r(x1-x2,y1-y2);
	v.Normalize();
	v = v*10;
	Vector2r n = Vector2r(-v[1],v[0]);
	
	glBegin(GL_TRIANGLES);
		glVertex2i(x2,y2);
		glVertex2i(x2+2*(int)v[0]+(int)n[0],y2+2*(int)v[1]+(int)n[1]);
		glVertex2i(x2+2*(int)v[0]-(int)n[0],y2+2*(int)v[1]-(int)n[1]);
	glEnd();

}


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
	emit verifyValidity();
}



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
			shared_ptr<GLWindow> glw = wm.getWindow(id);
			startX = glw->getMinX()+glw->getSizeX()/2;
			startY = glw->getMinY()+glw->getSizeY()/2;
			endX = e->x();
			endY = e->y();
		}
		else
		{
			relation.first = -1;
			relation.second = -1;
			startRelation = false;
		}
	}
	else if ((e->state() & Qt::KeyButtonMask)==Qt::ShiftButton)
	{
		selectedEngine = selectEngine(e->x(), e->y());
		emit engineSelected(selectedEngine);
		if (selectedEngine==-1)
			relationSelected = selectRelation(e->x(), e->y(), 3.0, selectedRelation.first, selectedRelation.second);
	}
	else if (wm.mousePressEvent(e->x(),e->y())==-1)
		QGLViewer::mousePressEvent(e);

	updateGL();
	emit verifyValidity();
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
	emit verifyValidity();
}


void GLEngineEditor::mouseDoubleClickEvent(QMouseEvent *e)
{
	//if (wm.mouseDoubleClickEvent(e->x(),e->y())==-1)
	//	QGLViewer::mouseDoubleClickEvent(e);

	
	int tmp = wm.getPointedWindow(e->x(),e->y());
	if (firstEngine!=-1)
		wm.getWindow(firstEngine)->setBackgroundColor(savedColor[0],savedColor[1],savedColor[2]);
	
	firstEngine = tmp;
	if (firstEngine!=-1)
	{		
		wm.getWindow(firstEngine)->getBackgroundColor(savedColor[0],savedColor[1],savedColor[2]);
		wm.getWindow(firstEngine)->setBackgroundColor(1,1,0.5);
	}
	
	updateGL();
	emit verifyValidity();
}


void GLEngineEditor::keyPressEvent(QKeyEvent *e)
{	
	if (e->key()==Qt::Key_Backspace || e->key()==Qt::Key_Delete)
	{
		if (relationSelected)
		{
			loop.erase(selectedRelation);
			relationSelected=false;
		}
		else if (selectedEngine!=-1)
		{
			//if (relationSelected && (selectedRelation.first==selectedEngine || selectedRelation.second==selectedEngine))
			//	relationSelected=false;
		
			set<pair<int,int>, lessThanPair >::iterator li = loop.begin();
			set<pair<int,int>, lessThanPair >::iterator liEnd = loop.end();
			for( ; li!=liEnd ; ++li)
				if ((*li).first==selectedEngine || (*li).second==selectedEngine)
					loop.erase(*li);

			wm.deleteWindow(selectedEngine);

			emit deleteEngine(selectedEngine);

			if (firstEngine==selectedEngine)
				firstEngine=-1;
			selectedEngine = -1;
		}
	}

	updateGL();
	emit verifyValidity();
}


void GLEngineEditor::resizeEvent(QResizeEvent * e)
{
	wm.resizeEvent(e->size().width(),e->size().height());
	QGLViewer::resizeEvent(e);
}


void GLEngineEditor::updateLabel(int i)
{
	shared_ptr<GLTextLabel> tl = YADE_PTR_CAST<GLTextLabel>(wm.getWindow(i));
	//tl->setText(data->fracProperties[i]->name);
	//tl->setBorderColor(data->fractures[i]->color[0],data->fractures[i]->color[1],data->fractures[i]->color[2]);
	tl->fitTextSize();
	updateGL();
}


void GLEngineEditor::cutSegmentWithRectangles(int& x1,int& y1,int& x2,int& y2, int label1, int label2)
{
	Vector2r box[4];
	shared_ptr<GLWindow> glw;

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


bool GLEngineEditor::selectRelation(int x, int y, float threshold, int &a, int &b)
{

	shared_ptr<GLWindow> glw1,glw2;
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

	if (selected)
		selectedEngine = -1;

	return selected;
}


int GLEngineEditor::createNewTextLabel(const string& name, float r, float g, float b )
{
	shared_ptr<GLTextLabel> tl(new GLTextLabel());
	tl->setText(const_cast<char*>(name.c_str()));
	tl->setTextColor(0,0,0);
	tl->setBorderColor(1,0,1);
	tl->setBackgroundColor(r,g,b);
	tl->fitTextSize();
	tl->setResizable(false);
	tl->setWinTranslucenty(0.7);
	tl->setBorderWidth(2);

	tl->setMinX(10);
	tl->setMinY(10);	

	shared_ptr<GLWindowsManager::EventSubscription> es(new GLWindowsManager::EventSubscription());
	es->mouseDoubleClick = false;
	return wm.addWindow(tl,es);
}


int GLEngineEditor::addEngine(const string& engineName )
{
	int id = createNewTextLabel(engineName,0,0.67,1);

	updateGL();
	emit verifyValidity();

	return id;
}


int GLEngineEditor::addDeusExMachina	(const string& engineName )
{
	int id = createNewTextLabel(engineName,0.67,0.33,1);

	updateGL();
	emit verifyValidity();

	return id;
}


int GLEngineEditor::addMetaDispatchingEngine2D(const string& engineName, const string& engineUnitName, const string& baseClass1Name,  const string& baseClass2Name)
{
	int id = createNewTextLabel(engineName,0,1,0.5);

	updateGL();
	emit verifyValidity();

	return id;
}


int GLEngineEditor::addMetaDispatchingEngine1D(const string& engineName, const string& engineUnitName, const string& baseClass1Name)
{
	int id = createNewTextLabel(engineName,0,1,0.5);

	updateGL();
	emit verifyValidity();

	return id;
}


int GLEngineEditor::selectEngine(int x, int y)
{
	int selected = wm.getPointedWindow(x,y);

	if (selected!=-1)
		relationSelected = false;

	return selected;
}


bool GLEngineEditor::verify(string& errorMessage)
{
	errorMessage = "OK : You can save now";

	// for each n engine should have only n arrows
	if (wm.nbWindows()==1)
		return true;
	
	if (wm.nbWindows()==0)
	{
		errorMessage = "ERROR : No engine added";
		return false;
	}
	
	if (firstEngine==-1)
	{
		errorMessage = "ERROR : No initial engine";
		return false;
	}
	
	if (loop.size()!=wm.nbWindows())
	{
		errorMessage = "ERROR : No loop defined";
		return false;
	}

	int last = firstEngine;
	// for each engine we should have only 1 incoming and 1 outcoming arrow
	for(unsigned int i=0;i<wm.nbWindows();i++) 
	{
		last = findRelationStartingWith(last);
		if (last==-1)
		{
			errorMessage = "ERROR : No loop defined";
			return false;
		}
	}

	return true;
}


int GLEngineEditor::findRelationStartingWith(int first)
{
	set<pair<int,int>, lessThanPair >::iterator li    =  loop.begin();
	set<pair<int,int>, lessThanPair >::iterator liEnd =  loop.end();

	for( ; li!=liEnd ; ++li)
	{
		if ((*li).first == first)
			return (*li).second;
	}

	return -1;
}

