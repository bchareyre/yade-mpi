/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef TETRAHEDRON2INTERACTINGMYTETRAHEDRON_HPP
#define TETRAHEDRON2INTERACTINGMYTETRAHEDRON_HPP

#include<yade/pkg-common/InteractingGeometryEngineUnit.hpp>

/*! 
 * \brief This class exists to create an InteractingMyTetrahedron from Tetrahedron
 *
 * Vaclav: here you will create a representation of tetrahedron that you want to use
 * for calculating interactions. The simplest example is to use four spheres.
 *  - each sphere's center is a midpoint between the tetrahedron's node and tetrahedron's center
 *  - each sphere's radius is equal to the distance from this midpoint to the tetrahedron's center (and node)
 *
 * You may want to use any other representation. Even a tetrahedron, but then
 * be catious when you calculate forces when collision is with the edge. Maybe
 * you will have to compute not penetrationDepth of the collision, but the
 * overlapping volume. Becasue the colliding force will be linear proportional
 * not to the penetrationDepth but to the amount of volume?
 *
 * it's your choince :)
 *
*/

class Tetrahedron2InteractingMyTetrahedron : public InteractingGeometryEngineUnit
{
	public :
		void go(	  const shared_ptr<GeometricalModel>& gm
				, shared_ptr<InteractingGeometry>& ig
				, const Se3r& se3
				, const Body*	);

	FUNCTOR2D(Tetrahedron,InteractingMyTetrahedron);
	REGISTER_CLASS_NAME(Tetrahedron2InteractingMyTetrahedron);
	REGISTER_BASE_CLASS_NAME(InteractingGeometryEngineUnit);
	
	DEFINE_FUNCTOR_ORDER_2D(Tetrahedron,InteractingMyTetrahedron);
};

REGISTER_SERIALIZABLE(Tetrahedron2InteractingMyTetrahedron,false);

#endif // TETRAHEDRON2POLYHEDRALSWEPTSPHERE_HPP


