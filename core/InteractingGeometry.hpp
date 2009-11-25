/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/lib-serialization/Serializable.hpp>
#include<yade/lib-multimethods/Indexable.hpp>

#define BV_FUNCTOR_CACHE

class BoundingVolumeEngineUnit;

class InteractingGeometry : public Serializable, public Indexable
{
	public :
		InteractingGeometry(): diffuseColor(Vector3r(1,1,1)), wire(false), highlight(false) {}
		Vector3r diffuseColor;
		bool wire;
		bool highlight;

		#ifdef BV_FUNCTOR_CACHE
			shared_ptr<BoundingVolumeEngineUnit> boundFunctor;
		#endif

/// Serialization
	REGISTER_ATTRIBUTES(Serializable,(diffuseColor)(wire)(highlight));
	REGISTER_CLASS_AND_BASE(InteractingGeometry,Serializable Indexable);
/// Indexable
	REGISTER_INDEX_COUNTER(InteractingGeometry);
};
REGISTER_SERIALIZABLE(InteractingGeometry);

