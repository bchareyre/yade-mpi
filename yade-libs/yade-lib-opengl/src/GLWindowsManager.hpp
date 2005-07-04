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

	protected : std::vector<GLWindow*> windows;
	protected : std::vector<EventSubscription*> subscriptions;
	protected : std::vector<int> order;

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

	public : void addWindow(GLWindow * w, EventSubscription * s);
	public : void deleteWindow(int i);
	public : void moveWindowOnTop(int i);

	public : int getPointedWindow(int x,int y);
	public : GLWindow * getWindow(int i) { return windows[i]; };
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __GLWINDOWSMANAGER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
