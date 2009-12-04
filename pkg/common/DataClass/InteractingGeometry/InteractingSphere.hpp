/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/Shape.hpp>

class InteractingSphere : public Shape
{
	public :
		Real radius;

		InteractingSphere();
		InteractingSphere(Real _radius): radius(_radius){}
		virtual ~InteractingSphere ();

	REGISTER_ATTRIBUTES(Shape,(radius));
	REGISTER_CLASS_NAME(InteractingSphere);
	REGISTER_BASE_CLASS_NAME(Shape);
	REGISTER_CLASS_INDEX(InteractingSphere,Shape);
};

REGISTER_SERIALIZABLE(InteractingSphere);


