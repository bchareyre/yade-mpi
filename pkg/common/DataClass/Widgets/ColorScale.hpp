/*************************************************************************
*  Copyright (C) 2009 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/GeometricalModel.hpp>

class ColorScale : public GeometricalModel {
   public :
   Real posX,posY;
   Real width,height;
	string title;

	vector<Vector3r> colors;
	vector<string>	 labels;

	ColorScale ();
	virtual ~ColorScale ();

	REGISTER_ATTRIBUTES(GeometricalModel,(posX)(posY)(width)(height)(title));
   REGISTER_CLASS_NAME(ColorScale);
   REGISTER_BASE_CLASS_NAME(GeometricalModel);
   REGISTER_CLASS_INDEX(ColorScale,GeometricalModel);
};

REGISTER_SERIALIZABLE(ColorScale);


