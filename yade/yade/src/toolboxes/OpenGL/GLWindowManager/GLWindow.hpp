#ifndef __GLWINDOW_H__
#define __GLWINDOW_H__

#ifndef NULL
	#define NULL 0
#endif

class GLWindow
{	
	protected : int minX;
	public : int getMinX() { return minX; };
	public : void setMinX(int m) { minX = m; };

	protected : int minY;
	public : int getMinY() { return minY; };
	public : void setMinY(int m) { minY = m; };
	
	protected : int sizeX;
	public : int getSizeX() { return sizeX; };

	protected : int sizeY;
	public : int getSizeY() { return sizeY; };

	protected : int oldX;
	protected : int oldY;
	protected : bool mousePress;
	protected : bool displayed;
	protected : int selectedComponent;

	protected : bool resizable;
	public    : void setResizable(bool b) { resizable = b;};	
	public    : bool isResizable() { return resizable; };

	protected : int minSizeX;
	public    : void setMinimumWidth(int w) { minSizeX = w;};
	protected : int maxSizeX;
	public    : void setMaximumWidth(int w) { maxSizeX = w;};
	protected : int minSizeY; 	
	public    : void setMinimumHeight(int h) { minSizeY = h;};
	protected : int maxSizeY;
	public    : void setMaximumHeight(int h) { maxSizeY = h;};

				
	protected : int borderWidth;
	public    : void setBorderWidth(int w) { borderWidth = w;};

	protected : int threshold;
	public    : void setThreshold(int t) { threshold = t;};
	protected : float color[3];
	public    : void setBackgroundColor(float r, float g, float b) { color[0] = r; color[1] = g; color[2] = b;};	
	protected : float selectionColor[3];
	public    : void setSelectionColor(float r, float g, float b) { selectionColor[0] = r; selectionColor[1] = g; selectionColor[2] = b;};		
	protected : float borderColor[3];
	public    : void setBorderColor(float r, float g, float b) { borderColor[0] = r; borderColor[1] = g; borderColor[2] = b;};			
	protected : float winTranslucenty;
	public    : void setWinTranslucenty(float t) { winTranslucenty = t;};	
	
	//	1     5     2
	//	  *-------*
	//	  |	      |	
	// 	8 |   9   | 6
	//	  |	      |
	//	  *-------*
	//  4     7     3
	public : bool selectComponent(int x,int y);
		
	private : void drawSelectedComponent();
	private : void drawWindow();
	
	protected : virtual void glDrawInsideWindow() = 0;
	
	// construction
	public : GLWindow (int minX=10, int minY=10, int sizeX=10, int sizeY=10);
	public : virtual ~GLWindow ();
	
	public : void swapDisplayed();	
	public : void glDraw();
	
	public : bool mouseMoveEvent(int x, int y);
	public : bool mousePressEvent(int x, int y);
	public : bool mouseReleaseEvent(int x, int y);
	public : bool mouseDoubleClickEvent(int x, int y);
	
	public : bool mouseIsOnWindows(int x,int y);
};

#endif // __GLWINDOW_H__
