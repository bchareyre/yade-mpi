/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLWindowsManager.hpp"
#include <GL/gl.h>
#include <iostream>

using namespace std;


GLWindowsManager::GLWindowsManager()
{
	windows.clear();
	selectedWindow = -1;

	width = 0;
	height = 0;

}


GLWindowsManager::~GLWindowsManager ()
{

}


void GLWindowsManager::glDraw()
{	

	glPushAttrib(GL_ALL_ATTRIB_BITS);
			
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0, width, height, 0, -1, 1 );
  
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_LIGHTING);

	set<shared_ptr<WindowDescription>, lessThanWindow >::reverse_iterator oi    = orderedWindows.rbegin();
	set<shared_ptr<WindowDescription>, lessThanWindow >::reverse_iterator oiEnd = orderedWindows.rend();
	for( ; oi!=oiEnd ; ++oi)
		(*oi)->window->glDraw();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix(); 

	glMatrixMode(GL_PROJECTION); 
	glPopMatrix();
	
	glPopAttrib();

}


int GLWindowsManager::mouseMoveEvent(int x, int y)
{
	if (selectedWindow==-1)
	{
		set<shared_ptr<WindowDescription>, lessThanWindow >::iterator oi    = orderedWindows.begin();
		set<shared_ptr<WindowDescription>, lessThanWindow >::iterator oiEnd = orderedWindows.end();
		bool found = false;
		for( ; oi!=oiEnd && !found; ++oi)
			found = (*oi)->window->selectComponent(x,y);
	}
	else if (windows[selectedWindow]->eventSubscription->mouseMove && windows[selectedWindow]->window->mouseMoveEvent(x,y))
		return selectedWindow;
	
	return -1;
}


int GLWindowsManager::mousePressEvent(int x, int y)
{
	set<shared_ptr<WindowDescription>, lessThanWindow >::iterator oi    = orderedWindows.begin();
	set<shared_ptr<WindowDescription>, lessThanWindow >::iterator oiEnd = orderedWindows.end();	
	for(; oi!=oiEnd ; ++oi)
	{
		if ((*oi)->eventSubscription->mousePress && (*oi)->window->mousePressEvent(x,y))
		{			
			selectedWindow = (*oi)->id;
			moveWindowOnTop(*oi);
			return selectedWindow;
		}
	}

	selectedWindow = -1;
	return selectedWindow;
}


int GLWindowsManager::mouseReleaseEvent(int x, int y)
{
	selectedWindow = -1;	

	if (selectedWindow!=-1)
	{		
		windows[selectedWindow]->window->mouseReleaseEvent(x,y);
		int tmpS = selectedWindow;
		selectedWindow = -1;

		set<shared_ptr<WindowDescription>, lessThanWindow >::iterator oi    = orderedWindows.begin();
		set<shared_ptr<WindowDescription>, lessThanWindow >::iterator oiEnd = orderedWindows.end();	
		for(; oi!=oiEnd ; ++oi)
		{
			if ((*oi)->eventSubscription->mouseRelease && (*oi)->window->mouseReleaseEvent(x,y))
			{	
				int tmp = (*oi)->id;
				moveWindowOnTop(*oi);
				return tmp;
			}
		}
		
		return tmpS;
	}

	return -1;
}


int GLWindowsManager::mouseDoubleClickEvent(int x, int y)
{
	set<shared_ptr<WindowDescription>, lessThanWindow >::iterator oi    = orderedWindows.begin();
	set<shared_ptr<WindowDescription>, lessThanWindow >::iterator oiEnd = orderedWindows.end();	
	for(; oi!=oiEnd ; ++oi)
	{
		if ((*oi)->eventSubscription->mouseDoubleClick && (*oi)->window->mouseDoubleClickEvent(x,y))
		{
			selectedWindow = (*oi)->id;
			moveWindowOnTop(*oi);
			return selectedWindow;
		}
	}

	selectedWindow = -1;
	return -1;
}


void GLWindowsManager::resizeEvent(int w, int h)
{
	width = w;
	height = h;
	set<shared_ptr<WindowDescription>, lessThanWindow >::iterator oi    = orderedWindows.begin();
	set<shared_ptr<WindowDescription>, lessThanWindow >::iterator oiEnd = orderedWindows.end();	
	for(; oi!=oiEnd ; ++oi)
		(*oi)->window->resizeGlWindow(w,h);
}


int GLWindowsManager::addWindow(shared_ptr<GLWindow> w, shared_ptr<EventSubscription> s)
{
	w->resizeGlWindow(width,height);
	shared_ptr<WindowDescription> wd(new WindowDescription);
	wd->window = w;
	wd->eventSubscription = s;

	wd->id=0;
	wd->order=0;

	map<int,shared_ptr<WindowDescription> >::iterator wi    = windows.begin();
	map<int,shared_ptr<WindowDescription> >::iterator wiEnd = windows.end();
	for( ; wi!=wiEnd ; ++wi)
	{
		if ((*wi).second->id>=wd->id)
			wd->id = (*wi).second->id+1;
		if ((*wi).second->order>=wd->id)
			wd->id = (*wi).second->order+1;
	}

	windows[wd->id] = wd;

	moveWindowOnTop(wd);

	return wd->id;
}


void GLWindowsManager::deleteWindow(int i)
{
	windows.erase(i);
	rebuildOrderedWindowsList();
}


void GLWindowsManager::moveWindowOnTop(const shared_ptr<WindowDescription>& wd)
{
	map<int, shared_ptr<WindowDescription> >::iterator wi    = windows.begin();
	map<int, shared_ptr<WindowDescription> >::iterator wiEnd = windows.end();
	for( ; wi!=wiEnd ; ++wi)
		((*wi).second->order)++;

	windows[wd->id]->order = 0;

	rebuildOrderedWindowsList();
}


int GLWindowsManager::getPointedWindow(int x,int y)
{
	set<shared_ptr<WindowDescription>, lessThanWindow >::iterator oi    = orderedWindows.begin();
	set<shared_ptr<WindowDescription>, lessThanWindow >::iterator oiEnd = orderedWindows.end();	
	for(; oi!=oiEnd ; ++oi)
	{
		if ((*oi)->window->mouseIsOnWindows(x,y))
			return (*oi)->id;
	}

	return -1;
}


shared_ptr<GLWindow> GLWindowsManager::getWindow(int i)
{
	return windows[i]->window;
}


void GLWindowsManager::rebuildOrderedWindowsList()
{
	orderedWindows.clear();
	map<int,shared_ptr<WindowDescription> >::iterator wi    = windows.begin();
	map<int,shared_ptr<WindowDescription> >::iterator wiEnd = windows.end();
	for( ; wi!=wiEnd ; ++wi)
		orderedWindows.insert((*wi).second);
}


unsigned int GLWindowsManager::nbWindows()
{
	return windows.size();
}


