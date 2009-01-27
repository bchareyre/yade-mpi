/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include "GLWindow.hpp"
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>

class GLTextLabel : public GLWindow
{	
	
	private :
		char * text;
		Vector3r textColor;

		void glDrawInsideWindow();
		void drawString(char * str,int ,int ,Vector3r c);

	// construction
	public :
		GLTextLabel (int minX=10, int minY=10, int sizeX=10, int sizeY=10,char * text=0);
		void setText(char * text);
		void fitTextSize();
		void setTextColor(float r,float g,float b);

		~GLTextLabel ();
};


