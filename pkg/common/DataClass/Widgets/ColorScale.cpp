/*************************************************************************
*  Copyright (C) 2009 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ColorScale.hpp"

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


YADE_PLUGIN((ColorScale));