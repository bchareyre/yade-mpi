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

#ifndef __QGLSUBWINDOW_H__
#define __QGLSUBWINDOW_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <QGLViewer/qglviewer.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class QGLSubWindow
{	
	protected : int minX;
	protected : int minY;
	protected : int sizeX;
	protected : int sizeY;
	protected : int oldX;
	protected : int oldY;
	protected : bool mousePress;
	protected : bool displayed;
	protected : int selected;
	
	protected : int minSizeX;
	public    : void setMinimumWidth(int w) { minSizeX = w;};
	protected : int maxSizeX;
	public    : void setMaximumWidth(int w) { maxSizeX = w;};
	protected : int minSizeY; 	
	public    : void setMinimumHeight(int h) { minSizeY = h;};
	protected : int maxSizeY;
	public    : void setMaximumHeight(int h) { maxSizeY = h;};
	
	protected : int threshold;
	public    : void setThreshold(int t) { threshold = t;};
	protected : float color[3];
	public    : void setBackgroundColor(float r, float g, float b) { color[0] = r; color[1] = g; color[2] = b;};	
	protected : float selectionColor[3];
	public    : void setSelectionColor(float r, float g, float b) { selectionColor[0] = r; selectionColor[1] = g; selectionColor[2] = b;};		
	protected : float winTranslucenty;
	public    : void setWinTranslucenty(float t) { winTranslucenty = t;};	
	
	//	1     5     2
	//	  *-------*
	//	  |	  |	
	// 	8 |   9   | 6
	//	  |	  |
	//	  *-------*
	//      4     7     3
	protected : bool selectedComponent(int x,int y);
		
	private : QGLViewer * glViewer;
	private : void drawSelectedComponent();
	private : void drawWindow();
	
	protected : virtual void glDrawInsideWindow() = 0;
	
	// construction
	public : QGLSubWindow (QGLViewer * glViewer, int minX=10, int minY=10, int sizeX=100, int sizeY=100);
	public : virtual ~QGLSubWindow ();
	
	public : void swapDisplayed();	
	public : void glDraw();
	
	public : bool mouseMoveEvent(QMouseEvent * e);
	public : bool mousePressEvent(QMouseEvent *e);
	public : bool mouseReleaseEvent(QMouseEvent *e);
	public : bool mouseDoubleClickEvent(QMouseEvent *e);

	
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __QGLSUBWINDOW_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
