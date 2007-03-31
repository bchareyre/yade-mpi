/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLWINDOW_HPP
#define GLWINDOW_HPP

class GLWindow
{	
	protected :
		int		 glWindowWidth
				,glWindowHeight
				,minX
				,minY
				,sizeX
				,sizeY
				,oldX
				,oldY
				,selectedComponent
				,minSizeX
				,maxSizeX
				,minSizeY
				,maxSizeY
				,borderWidth
				,threshold;

		float		 color[3]
				,selectionColor[3]
				,borderColor[3]
				,winTranslucenty;

		bool		 mousePress
				,displayed
				,resizable;

		virtual void glDrawInsideWindow() = 0;
	
	private :
		void drawSelectedComponent();
		void drawWindow();
	

	public :
		void resizeGlWindow(int w,int h);
		void setResizable(bool b) { resizable = b;};	
		bool isResizable() { return resizable; };
		int getMinX() { return minX; };
		void setMinX(int m) { minX = m; };
		int getMinY() { return minY; };
		void setMinY(int m) { minY = m; };
		int getSizeX() { return sizeX; };
		int getSizeY() { return sizeY; };
		
		void setMinimumWidth(int w) { minSizeX = w;};
		void setMaximumWidth(int w) { maxSizeX = w;};
		void setMinimumHeight(int h) { minSizeY = h;};
		void setMaximumHeight(int h) { maxSizeY = h;};
		    	
		void setBorderWidth(int w) { borderWidth = w;};
		void setThreshold(int t) { threshold = t;};
		
		void setBackgroundColor(float r, float g, float b) { color[0] = r; color[1] = g; color[2] = b;};	
		void getBackgroundColor(float& r, float& g, float& b) { r=color[0]; g=color[1]; b=color[2];};	
		void getBackgroundColor(double& r, double& g, double& b) { r=color[0]; g=color[1]; b=color[2];};	
		
		void setSelectionColor(float r, float g, float b) { selectionColor[0] = r; selectionColor[1] = g; selectionColor[2] = b;};		
		void setBorderColor(float r, float g, float b) { borderColor[0] = r; borderColor[1] = g; borderColor[2] = b;};			
		void setWinTranslucenty(float t) { winTranslucenty = t;};	
	
		//	1     5     2
		//	  *-------*
		//	  |	      |	
		// 	8 |   9   | 6
		//	  |	      |
		//	  *-------*
		//  4     7     3
		bool selectComponent(int x,int y);
	
		GLWindow (int minX=10, int minY=10, int sizeX=10, int sizeY=10);
		virtual ~GLWindow ();
	
		void swapDisplayed();	
		void glDraw();
		
		bool mouseMoveEvent(int x, int y);
		bool mousePressEvent(int x, int y);
		bool mouseReleaseEvent(int x, int y);
		bool mouseDoubleClickEvent(int x, int y);
		
		bool mouseIsOnWindows(int x,int y);
};

#endif //  GLWINDOW_HPP

