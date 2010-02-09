/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/Shape.hpp>


class Box: public Shape{
	public:
		Box(const Vector3r& _extents): extents(_extents){}
		virtual ~Box ();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(Box,Shape,"Box (cuboid) particle geometry. (Avoid using in new code, prefer :yref:`Facet` instead.",
		((Vector3r,extents,,"Half-size of the cuboid")),
		/* ctor */ createIndex();
	);
	REGISTER_CLASS_INDEX(Box,Shape);
};
REGISTER_SERIALIZABLE(Box);


