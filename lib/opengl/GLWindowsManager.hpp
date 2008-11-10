/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLWINDOWSMANAGER_HPP
#define GLWINDOWSMANAGER_HPP

#include "GLWindow.hpp"
#include <vector>
#include <map>
#include <set>
#ifndef  __GXX_EXPERIMENTAL_CXX0X__
#	include<boost/shared_ptr.hpp>
	using boost::shared_ptr;
#else
#	include<memory>
	using std::shared_ptr;
#endif


using namespace std;
using namespace boost;

class GLWindowsManager
{	
	private :
		void rebuildOrderedWindowsList();

	public :
		struct EventSubscription
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
		};

		struct WindowDescription
		{
			shared_ptr<GLWindow> window;
			shared_ptr<EventSubscription> eventSubscription;
			int order;
			int id;
		};

		struct lessThanWindow
		{
			bool operator()(const shared_ptr<WindowDescription>& w1,const shared_ptr<WindowDescription>& w2)
			{
				return w1->order<w2->order;
			}
		};


	protected :
		map<int,shared_ptr<WindowDescription> > windows;
		set<shared_ptr<WindowDescription>, lessThanWindow > orderedWindows;
		int selectedWindow;

	public :
		int width;
		int height;
	
		GLWindowsManager ();
		~GLWindowsManager ();
		
		void glDraw();
		
		int mouseMoveEvent(int x, int y);
		int mousePressEvent(int x, int y);
		int mouseReleaseEvent(int x, int y);
		int mouseDoubleClickEvent(int x, int y);
	
		void resizeEvent(int w, int h);
	
		int addWindow(shared_ptr<GLWindow> w, shared_ptr<EventSubscription> s);
		void deleteWindow(int i);
		void moveWindowOnTop(const shared_ptr<WindowDescription>& wd);
	
		int getPointedWindow(int x,int y);
		shared_ptr<GLWindow> getWindow(int i);
	
		unsigned int nbWindows();

};

#endif //  GLWINDOWSMANAGER_HPP

