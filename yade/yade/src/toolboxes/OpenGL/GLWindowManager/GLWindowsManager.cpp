#include "GLWindowsManager.hpp"

#ifdef WIN32
	#include <windows.h>
#endif
#include <GL/gl.h>


GLWindowsManager::GLWindowsManager()
{
	windows.clear();
	subscriptions.clear();
	selectedWindow = -1;
	order.clear();

	width = 0;
	height = 0;

}

GLWindowsManager::~GLWindowsManager ()
{
	for(unsigned int i=0;i<windows.size();i++)
	{
		delete windows[i];
		delete subscriptions[i];
	}
	windows.clear();
	subscriptions.clear();
	order.clear();
	
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

	std::vector<int>::iterator oi	 = order.begin();
	std::vector<int>::iterator oiEnd = order.end();
	for( ; oi!=oiEnd ; ++oi)
		windows[(*oi)]->glDraw();	

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
		std::vector<int>::reverse_iterator oi		= order.rbegin();
		std::vector<int>::reverse_iterator oiEnd	= order.rend();	
		bool found = false;
		for(; oi!=oiEnd && !found; ++oi)
			found = windows[(*oi)]->selectComponent(x,y);		
	}
	else if (subscriptions[selectedWindow]->mouseMove && windows[selectedWindow]->mouseMoveEvent(x,y))
		return selectedWindow;
	
	return -1;
}

int GLWindowsManager::mousePressEvent(int x, int y)
{
	std::vector<int>::reverse_iterator oi		= order.rbegin();
	std::vector<int>::reverse_iterator oiEnd	= order.rend();
	
	for(; oi!=oiEnd ; ++oi)
	{
		int id = (*oi);
		if (subscriptions[id]->mousePress && windows[id]->mousePressEvent(x,y))
		{
			moveWindowOnTop(id);
			selectedWindow = id;
			return selectedWindow;
		}
	}

	selectedWindow = -1;
	return -1;
}

int GLWindowsManager::mouseReleaseEvent(int x, int y)
{	
	if (selectedWindow!=-1)
	{		
		windows[selectedWindow]->mouseReleaseEvent(x,y);
		int tmpS = selectedWindow;
		selectedWindow = -1;

		std::vector<int>::reverse_iterator oi		= order.rbegin();
		std::vector<int>::reverse_iterator oiEnd	= order.rend();
		for(; oi!=oiEnd ; ++oi)
		{
			int id = (*oi);
			if (subscriptions[id]->mouseRelease && windows[id]->mouseReleaseEvent(x,y))
			{
				moveWindowOnTop(id);
				return id;
			}
		}
		
		return tmpS;
	}

	return -1;
}


int GLWindowsManager::mouseDoubleClickEvent(int x, int y)
{
	std::vector<int>::reverse_iterator oi		= order.rbegin();
	std::vector<int>::reverse_iterator oiEnd	= order.rend();
	
	for(; oi!=oiEnd ; ++oi)
	{
		int id = (*oi);
		if (subscriptions[id]->mouseDoubleClick && windows[id]->mouseDoubleClickEvent(x,y))
		{
			moveWindowOnTop(id);
			selectedWindow = id;
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
	for(int i=0;i<windows.size();i++)
		windows[i]->resizeGlWindow(w,h);
}


void GLWindowsManager::addWindow(GLWindow * w, EventSubscription * s)
{
	windows.push_back(w);
	subscriptions.push_back(s);
	order.push_back(windows.size()-1);
}

void GLWindowsManager::deleteWindow(int i)
{
	delete subscriptions[i];
	delete windows[i];

	unsigned int j;
	
	for(j=i;j<windows.size()-1;j++)
	{
		windows[j] = windows[j+1];
		subscriptions[j] = subscriptions[j+1];		
	}
	
	int id;
	for(j=0;j<order.size()-1;j++)
		if (order[j]==i)
			id = j;

	for(j=id;j<order.size()-1;j++)
		order[j] = order[j+1];
			
	windows.resize(windows.size()-1);
	subscriptions.resize(subscriptions.size()-1);
	order.resize(order.size()-1);

	for(j=0;j<order.size();j++)
		if (order[j]>i)
			order[j]--;

}

void GLWindowsManager::moveWindowOnTop(int i)
{
	unsigned int j = 0;

	while (j<order.size() && order[j]!=i)
		j++;

	while (j<order.size()-1)
	{
		order[j] = order[j+1];
		j++;
	}

	order[j] = i;
}

int GLWindowsManager::getPointedWindow(int x,int y)
{
	std::vector<int>::reverse_iterator oi		= order.rbegin();
	std::vector<int>::reverse_iterator oiEnd	= order.rend();
	
	for(; oi!=oiEnd ; ++oi)
	{
		int id = (*oi);
		if (windows[id]->mouseIsOnWindows(x,y))
			return id;
	}

	return -1;
}
