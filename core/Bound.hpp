/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once

#include<yade/lib-base/Math.hpp>
#include<yade/lib-serialization/Serializable.hpp>
#include<yade/lib-multimethods/Indexable.hpp>
#include<yade/core/Dispatcher.hpp>

/*! Interface for approximate body locations in space

	Note: the min and max members refer to shear coordinates, in periodic
	and sheared space, not cartesian coordinates in the physical space.

*/

class Bound: public Serializable, public Indexable{
	public:
		Vector3r min,max;
	YADE_CLASS_BASE_DOC_ATTRS_DEPREC_INIT_CTOR_PY(Bound,Serializable,"Object bounding part of space taken by associated body; might be larger, used to optimalize collision detection",
		((Vector3r,color,Vector3r(1,1,1),"Color for rendering this object")),
		/*deprec*/ ((diffuseColor,color,"For consistency with Shape.color")),
		/* init */,
		/* ctor*/ min=max=Vector3r::Zero(),
		/*py*/
		YADE_PY_TOPINDEXABLE(Bound)
		// we want those to be accessible from python, but not serialized
		.def_readonly("min",&Bound::min,"Lower corner of box containing this bound (and the :yref:`Body` as well)")
		.def_readonly("max",&Bound::max,"Upper corner of box containing this bound (and the :yref:`Body` as well)")
	);
	REGISTER_INDEX_COUNTER(Bound);
};
REGISTER_SERIALIZABLE(Bound);
