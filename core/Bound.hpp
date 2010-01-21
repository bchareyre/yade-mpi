/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once

#include<yade/lib-base/yadeWm3Extra.hpp>
#include<yade/lib-serialization/Serializable.hpp>
#include<yade/lib-multimethods/Indexable.hpp>
#include<yade/core/Dispatcher.hpp>

/*! Interface for approximate body locations in space

	Note: the min and max members refer to shear coordinates, in periodic
	and sheared space, not cartesian coordinates in the physical space.

*/

class Bound : public Serializable, public Indexable
{
	public :
		Vector3r	 diffuseColor,min,max;
		Bound(): diffuseColor(Vector3r(1,1,1)), min(Vector3r(0,0,0)), max(Vector3r(0,0,0)) {}

	//REGISTER_ATTRIBUTES(Serializable,(diffuseColor));
	//REGISTER_CLASS_AND_BASE(Bound,Serializable Indexable);
	YADE_CLASS_BASE_ATTRS_PY(Bound,Serializable,(diffuseColor),
		YADE_PY_TOPINDEXABLE(Bound)
		.def_readonly("min",&Bound::min)
		.def_readonly("max",&Bound::max)
	);
	REGISTER_INDEX_COUNTER(Bound);
};
REGISTER_SERIALIZABLE(Bound);
