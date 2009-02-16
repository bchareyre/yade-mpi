/*************************************************************************
*  Copyright (C) 2009 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ColorScale.hpp"
#include<yade/lib-opengl/OpenGLWrapper.hpp>

ColorScale::ColorScale () : GeometricalModel()
{
	createIndex();
	posX=0;
	posY=0.2;
	width=0.05;
	height=0.5;
	title="";
}


ColorScale::~ColorScale ()
{
}


void ColorScale::registerAttributes()
{
	GeometricalModel::registerAttributes();
	REGISTER_ATTRIBUTE(posX);
	REGISTER_ATTRIBUTE(posY);
	REGISTER_ATTRIBUTE(width);
	REGISTER_ATTRIBUTE(height);
	REGISTER_ATTRIBUTE(title);
}


YADE_PLUGIN();
