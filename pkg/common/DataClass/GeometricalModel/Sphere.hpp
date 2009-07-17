/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/GeometricalModel.hpp>

class Sphere : public GeometricalModel
{
	public :
		Real radius;

		Sphere ();
		virtual ~Sphere ();

	REGISTER_ATTRIBUTES(GeometricalModel,(radius));
	REGISTER_CLASS_AND_BASE(Sphere,GeometricalModel);
	REGISTER_CLASS_INDEX(Sphere,GeometricalModel);
};

REGISTER_SERIALIZABLE(Sphere);


