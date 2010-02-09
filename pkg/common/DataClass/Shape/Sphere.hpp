/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/Shape.hpp>

class Sphere: public Shape{
	public:
		Sphere(Real _radius): radius(_radius){ createIndex(); }
		virtual ~Sphere ();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(Sphere,Shape,"Geometry of spherical particle.",
		((Real,radius,NaN,"Radius [m]")),
		createIndex(); /*ctor*/
	);
	REGISTER_CLASS_INDEX(Sphere,Shape);
};

REGISTER_SERIALIZABLE(Sphere);


