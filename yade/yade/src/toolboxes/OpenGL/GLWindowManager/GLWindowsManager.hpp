#ifndef __GLWINDOWSMANAGER_H__
#define __GLWINDOWSMANAGER_H__

#include "GLWindow.hpp"
#include <vector>

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

	public : static int width;
	public : static int height;
	
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

#endif // __GLWINDOWSMANAGER_H__
