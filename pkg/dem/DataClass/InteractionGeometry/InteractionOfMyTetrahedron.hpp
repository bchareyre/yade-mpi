/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTIONOFMYTETRAHEDRON_HPP
#define INTERACTIONOFMYTETRAHEDRON_HPP

#include <vector>

#include<yade/core/InteractionGeometry.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>

/*! 
 * \brief This class stores an information about current collision/interaction of two bodies
 * that can talk with the InteractingMyTetrahedron.
 *
 * "can talk" means that an EngineUnit is written for this pair of bodies, in this example I have written:
 *
 * InteractingMyTetrahedron2InteractingBox4InteractionOfMyTetrahedron
 * InteractingMyTetrahedron2InteractingMyTetrahedron4InteractionOfMyTetrahedron
 *
 * Following table shows currently plssible interactions:
 *
 *                           |  InteractingMyTetrahedron   InteractingBox   InteractingSphere
 * --------------------------+----------------------------------------------------------
 * InteractingMyTetrahedron  |             +                     +                 -
 * InteractingBox            |             +                     -                 -
 * InteractingSphere         |             -                     -                 -
 *
 * 
 * If you will write class
 *
 * InteractingMyTetrahedron2InteractingSphere4InteractionOfMyTetrahedron
 *
 * Then this interaction will become possible too. Just remember to register it
 * inside TetrahedronsTest in function createActors. And that's - your
 * tetrahedrons will be able to collide with spheres :)
 *
 *
 *
 *
 * Information about current interaction is here very simplified, so that I can
 * make this example quickly.
 *
 * In this example I am assuming that in general this interaction is
 * represented as a collision of two four-sphered bodies. Therefore I will
 * store here penetration depth for each of the spheres. Since in each body
 * four spheres are assumed to exist then I will have a 4x4 matrix of
 * penetration depths, a 4x4 matrix of normal directions of each penetration,
 * and a 4x4 matrix of contact points.
 *
 * So here you can see that the example interaction with InteractingBox will be
 * in fact emulating that the box contains four spheres. You can check that in
 * InteractingMyTetrahedron2InteractingBox4InteractionOfMyTetrahedron
 *
*/

class InteractionOfMyTetrahedron : public InteractionGeometry
{
	public :
		// if serialization was needed (REGISTER_ATTRIBUTE),
		// then I should use std::vector, because serialization is currently not working for C arrays
		// (will be fixed soon :)
		Real penetrationDepths[4][4];
		Vector3r contactPoints[4][4];
		Vector3r normals[4][4];
		
		InteractionOfMyTetrahedron();
		virtual ~InteractionOfMyTetrahedron();

	REGISTER_CLASS_NAME(InteractionOfMyTetrahedron);
	REGISTER_BASE_CLASS_NAME(InteractionGeometry);

	REGISTER_CLASS_INDEX(InteractionOfMyTetrahedron,InteractionGeometry);

};

REGISTER_SERIALIZABLE(InteractionOfMyTetrahedron);

#endif // INTERACTIONOFMYTETRAHEDRON_HPP

