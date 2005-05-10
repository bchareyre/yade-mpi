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

#ifndef __FPSTRACKER_H__
#define __FPSTRACKER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <list>
#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade-lib-time/Chrono.hpp>
#include "GLWindow.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class FpsTracker : public GLWindow
{	

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	private : Chrono chron;	
	
	private : std::list<std::pair<float,float> > fpss;
	private : int fpssSize;
	
	private : float fps;
	private : float moyFps;
	private : float minFps;
	private : float maxFps;
	private : int nbActions;		
	private : float lastTime;
	private : float currentTime;
	
	private : float curveColor[3];
	public  : void setCurveColor(float r, float g, float b) { curveColor[0] = r; curveColor[1] = g; curveColor[2] = b;};	
	private : float moyColor[3];	
	public  : void setMoyColor(float r, float g, float b) { moyColor[0] = r; moyColor[1] = g; moyColor[2] = b;};	
	private : int minNbActions;
	public  : void setMinNbActions(int n) { minNbActions = n;};	
	private : float minTime;
	public  : void setMinTime(float n) { minTime = n;};	
	private : float maxStoredPoints;
	public  : void setMaxStoredPoints(int n) { maxStoredPoints = n;};	
	
	private : void glDrawInsideWindow();
	private : void drawCurve();
	private : void updateMoy(float lastX, float lastY, float X, float Y);
	private : void drawString(string str,int x,int y,float * c);
	
	// construction
	public : FpsTracker (int minX=10, int minY=10, int sizeX=100, int sizeY=100);
	public : virtual ~FpsTracker ();
	public : inline void addOneAction() { nbActions++;};
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __FPSTRACKER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
