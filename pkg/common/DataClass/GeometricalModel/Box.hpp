/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#ifdef YADE_SHAPE

#include<yade/core/GeometricalModel.hpp>
class Box: public GeometricalModel{
	public:
		Vector3r extents;
		Box();
		virtual ~Box();
	REGISTER_ATTRIBUTES(GeometricalModel,(extents));
	REGISTER_CLASS_AND_BASE(Box,GeometricalModel);
	REGISTER_CLASS_INDEX(Box,GeometricalModel);
};

REGISTER_SERIALIZABLE(Box);

#else
	#error This file may be included only with the shape feature enabled.
#endif

