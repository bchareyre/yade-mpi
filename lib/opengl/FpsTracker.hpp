/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <list>
#include <vector>
#include <string>

#include "GLWindow.hpp"

using namespace std;

class FpsTracker : public GLWindow
{	
	private :
		std::list<std::pair<float,float> > fpss;
		int		fpssSize;
		
		float		 fps
				,moyFps
				,minFps
				,maxFps
				,lastTime
				,currentTime
				,curveColor[3]
				,moyColor[3]
				,minTime
				,maxStoredPoints;

		int		 nbActions
				,minNbActions;
		
		
		void glDrawInsideWindow();
		void drawCurve();
		void updateMoy(float lastX, float lastY, float X, float Y);
		void drawString(string str,int x,int y,float * c);
	
	public :
		void setCurveColor(float r, float g, float b) { curveColor[0] = r; curveColor[1] = g; curveColor[2] = b;};	
		void setMoyColor(float r, float g, float b) { moyColor[0] = r; moyColor[1] = g; moyColor[2] = b;};	
		void setMinNbActions(int n) { minNbActions = n;};	
		void setMinTime(float n) { minTime = n;};	
		void setMaxStoredPoints(int n) { maxStoredPoints = n;};	
	
		FpsTracker (int minX=10, int minY=10, int sizeX=100, int sizeY=100);
		virtual ~FpsTracker ();
		inline void addOneAction() { nbActions++;};
};


