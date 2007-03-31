/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTING_GEOMETRY_HPP
#define INTERACTING_GEOMETRY_HPP

#include<yade/lib-serialization/Serializable.hpp>
#include<yade/lib-multimethods/Indexable.hpp>

class BoundingVolume;

// the geometry through which the body will interact:
// - planet emitting gravity has just radius of influence
// - magnet has also just volume of influence
// - for tetrahedrons we can use sphere tree or sweptshpere volume
//
// in general we can use it to initialize interaction, and sometimes to terminate it
// (depending in which container it is stored).

class InteractingGeometry : public Serializable, public Indexable
{
	public :
		Vector3r diffuseColor;	// FIXME - why here? and then - why no
					// bool wire; even though GeometricalModel has bool wire ?
	
/// Serialization
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(InteractingGeometry);
	REGISTER_BASE_CLASS_NAME(Serializable Indexable);
	
/// Indexable
	REGISTER_INDEX_COUNTER(InteractingGeometry);
};

REGISTER_SERIALIZABLE(InteractingGeometry,false);

#endif //  INTERACTING_GEOMETRY_HPP

