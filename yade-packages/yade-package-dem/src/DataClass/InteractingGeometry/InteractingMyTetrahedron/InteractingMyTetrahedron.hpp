/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTING_MY_TETRAHEDRON_HPP
#define INTERACTING_MY_TETRAHEDRON_HPP

#include <yade/yade-core/InteractingGeometry.hpp>
#include <yade/yade-lib-wm3-math/Math.hpp>

/*! 
 * \brief This class stores an information about geometry for tetrahedron which
 * is used for calculation of interactions between other bodies's
 * InteractingGeometry.
 *
 * In this particular representation, the tertahedron is represented as four spheres
 * where each sphere is created using following method:
 *  - each sphere's center is a midpoint between the tetrahedron's node and tetrahedron's center
 *  - each sphere's radius is equal to the distance from this midpoint to the tetrahedron's center (and node)
 *
 *  
 *  Vaclav: you are supposed to change this class completely according to your
 *  decision, how you want to calculate interactions between objects. You may
 *  as well want to use just a Tetrahedron (four node's coordinates). In that
 *  case this class will be a mere duplication of class Tetrahedron
 *
 *  Spheres are just simpler so I prepared this example for you, using sphres.
 *  If you want to calculate forces between true tetrahedrons (not four
 *  spheres), then you can do that :)
 *
 *  When changing this class remember to update .cpp as well ;)
 *
*/

class InteractingMyTetrahedron : public InteractingGeometry
{
	public :
		Real		r1,r2,r3,r4; // radius of four spheres
		Vector3r	c1,c2,c3,c4; // centers of four spheres, relative to this Tetrahedron's center (physicalParameters->se3.position)

		InteractingMyTetrahedron ();
		virtual ~InteractingMyTetrahedron ();

/// Serialization
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(InteractingMyTetrahedron);
	REGISTER_BASE_CLASS_NAME(InteractingGeometry);

/// Indexable
	REGISTER_CLASS_INDEX(InteractingMyTetrahedron,InteractingGeometry);
};

REGISTER_SERIALIZABLE(InteractingMyTetrahedron,false);

#endif // INTERACTING_MY_TETRAHEDRON_HPP

