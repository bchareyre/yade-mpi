/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "FpsTracker.hpp"
#include "OpenGLWrapper.hpp"


#include <GL/glut.h>

#include <boost/lexical_cast.hpp>


using namespace boost;


FpsTracker::FpsTracker (int minX,int minY, int sizeX,int sizeY) : GLWindow(minX,minY,sizeX,sizeY)
{
//	chron.start();	
	lastTime	= 0;
	maxFps		= 0;
	minFps 		= 1000000;
	nbActions	= 0;
	displayed 	= false;

	setCurveColor(1.0,0.0,0.0);
	setMoyColor(0.0,0.0,1.0);	
	setMinNbActions(10);
	setMinTime(0.1);
	setMaxStoredPoints(2000);
	
	setWinTranslucenty(0.6);
		
	fpss.resize(0);
	fpssSize = 0;
}


FpsTracker::~FpsTracker ()
{

}


void FpsTracker::glDrawInsideWindow()
{	
	currentTime = 0;//chron.getTime();
		
	if (nbActions>=minNbActions && currentTime>=minTime)
	{			
		float fps = (float)nbActions/(currentTime-lastTime);
			
		if (minFps>fps)
			minFps = fps;
		else if (maxFps<fps)
			maxFps=fps;
				
		std::pair<float,float> lastPoint = (*(fpss.begin()));
			
		fpss.push_front(std::pair<float,float>(currentTime,fps));
		fpssSize++;
		if (fpssSize>maxStoredPoints)
		{
			fpssSize--;
			fpss.pop_back();
		}
			
		updateMoy(lastPoint.first,lastPoint.second,currentTime,fps);
		
		nbActions=0;
		lastTime = currentTime;
	}
		
	drawCurve();
}


void FpsTracker::updateMoy(float lastX, float lastY, float X, float Y)
{
	float width = X - lastX;
	float heightR=0, heightT=0;
	
	if (Y>lastY)
	{
		heightR = lastY;
		heightT = Y-lastY;
	}
	else if (Y<=lastY)
	{
		heightR = Y;
		heightT = lastY-Y;	
	}
	
	moyFps += width*(heightR+0.5*heightT);
}


void FpsTracker::drawCurve()
{
	string number;
	
	glDisable(GL_DEPTH_TEST);
		
	std::list<std::pair<float,float> >::iterator fi = fpss.begin();
	std::list<std::pair<float,float> >::iterator fiEnd = fpss.end();
		
	int maxPoints = sizeX/2;
	int i;
	for(i=0;i<fpssSize-1 && i<maxPoints ;i++)
		fi++;
	
	fiEnd = fi;
	fi = fpss.begin();
		
	float beginTime = (*fi).first;
	float endTime = (*fiEnd).first;
	fiEnd++;
	float elapsedTime = beginTime;

	float maxX = (float)(minX+sizeX);
	float maxY = (float)(minY+sizeY);
	
	float scale1 = (float)sizeY/(minFps-maxFps);
	float corr1 = maxY-scale1*minFps;
	
	float scale2 = (float)sizeX/(beginTime-endTime);
	float corr2 = maxX-scale2*beginTime;
			
	glColor3(0.3,0.3,0.3);
	glBegin(GL_LINES);
		for(i = (int)maxY-10 ; i>minY; i-=10)
		{	
			glVertex2i(minX,i);
			glVertex2i((int)maxX,i);
		}
	glEnd();	
	glBegin(GL_LINES);
		for(i = (int)maxX-10 ; i>minX; i-=10)
		{	
			glVertex2i(i,minY);
			glVertex2i(i,(int)maxY);
		}
	glEnd();
	
	
	number = lexical_cast<string>(minFps);
	drawString(number+" Hz",minX,(int)maxY+10,color);
	drawString(number+" Hz",minX,(int)maxY+12,color);
	number = lexical_cast<string>(maxFps);	
	drawString(number+" Hz",minX,minY-3,color);
	
	glColor3(curveColor[0],curveColor[1],curveColor[2]);
	glBegin(GL_LINE_STRIP);		
		for(;fi!=fiEnd;++fi)
			glVertex2f((*fi).first*scale2+corr2,(*fi).second*scale1+corr1);
	glEnd();	
	
	float currentFps = (*(fpss.begin())).second;
	number = lexical_cast<string>(currentFps);
	drawString(number+" Hz",(int)maxX+3,(int)(currentFps*scale1+corr1)+5,curveColor);
	
		
	float moy = moyFps/elapsedTime;
	if (moy>minFps && moy<maxFps)
	{
		glColor3(moyColor[0],moyColor[1],moyColor[2]);
		glBegin(GL_LINES);
			glVertex2f(minX,moy*scale1+corr1);
			glVertex2f(maxX,moy*scale1+corr1);
		glEnd();	
		number = lexical_cast<string>(moy);
		number += " Hz";
		drawString(number,minX-number.length()*7+2,(int)(moy*scale1+corr1)+5,moyColor);
	}
		
	glEnable(GL_DEPTH_TEST);	
}


void FpsTracker::drawString(string str,int x,int y,float * c)
{
	glPushMatrix();
	glRasterPos2i(x,y);
	glColor3fv(c);
	for(unsigned int i=0;i<str.length();i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
	glPopMatrix();
}

