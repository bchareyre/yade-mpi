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

#ifndef __GLWINDOWSMANAGER_H__
#define __GLWINDOWSMANAGER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "GLWindow.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <map>
#include <set>
#include <boost/shared_ptr.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace boost;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class GLWindowsManager
{	

	public : typedef struct EventSubscription
	{
		EventSubscription() 
		{
			mouseMove		= true;
			mousePress		= true;
			mouseRelease		= true;
			mouseDoubleClick	= true;
		};
		bool mouseMove;
		bool mousePress;
		bool mouseRelease;
		bool mouseDoubleClick;
	} EventSubscription;

	public : typedef struct WindowDescription
	{
		shared_ptr<GLWindow> window;
		shared_ptr<EventSubscription> eventSubscription;
		int order;
		int id;
	} WindowDescription;

	public : struct lessThanWindow
	{
		bool operator()(const shared_ptr<WindowDescription>& w1,const shared_ptr<WindowDescription>& w2)
		{
			return w1->order<w2->order;
		}
	} ;


	protected : map<int,shared_ptr<WindowDescription> > windows;
	protected : set<shared_ptr<WindowDescription>, lessThanWindow > orderedWindows;

	protected : int selectedWindow;

	public : int width;
	public : int height;
	
	// construction
	public : GLWindowsManager ();
	public : ~GLWindowsManager ();
	
	public : void glDraw();
	
	public : int mouseMoveEvent(int x, int y);
	public : int mousePressEvent(int x, int y);
	public : int mouseReleaseEvent(int x, int y);
	public : int mouseDoubleClickEvent(int x, int y);

	public : void resizeEvent(int w, int h);

	public : int addWindow(shared_ptr<GLWindow> w, shared_ptr<EventSubscription> s);
	public : void deleteWindow(int i);
	public : void moveWindowOnTop(const shared_ptr<WindowDescription>& wd);

	public : int getPointedWindow(int x,int y);
	public : shared_ptr<GLWindow> getWindow(int i);

	private : void rebuildOrderedWindowsList();
	public : unsigned int nbWindows();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __GLWINDOWSMANAGER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
