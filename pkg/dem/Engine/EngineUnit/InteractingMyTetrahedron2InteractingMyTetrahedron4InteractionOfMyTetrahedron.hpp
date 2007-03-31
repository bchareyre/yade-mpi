/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTINGMYTETRAHEDRON2INTERACTINGMYTETRAHEDRON4INTERACTIONOFMYTETRAHEDRON_HPP
#define INTERACTINGMYTETRAHEDRON2INTERACTINGMYTETRAHEDRON4INTERACTIONOFMYTETRAHEDRON_HPP

#include<yade/pkg-common/InteractionGeometryEngineUnit.hpp>

/*! 
 * \brief This class exists for the sole purpose of creating InteractionOfMyTetrahedron
 * from two colliding InteractingMyTetrahedron(s)
 *
 * so it must loop through all four x four spheres, and calculate for each of them a
 * penetration depth and normal direction. Thus filling 4x4 matrix.
 *
 * Vaclav: For example if you decide that InteractingMyTetrahedron is a real
 * tetrahedron, then maybe instead of calculating penetrationDepths you will
 * want to calculate the overlapping volume.
 *
*/

class InteractingMyTetrahedron2InteractingMyTetrahedron4InteractionOfMyTetrahedron : public InteractionGeometryEngineUnit
{
	public :
		virtual bool go(	const shared_ptr<InteractingGeometry>& cm1,
					const shared_ptr<InteractingGeometry>& cm2,
					const Se3r& se31,
					const Se3r& se32,
					const shared_ptr<Interaction>& c);

		virtual bool goReverse(	const shared_ptr<InteractingGeometry>& cm1,
					const shared_ptr<InteractingGeometry>& cm2,
					const Se3r& se31,
					const Se3r& se32,
					const shared_ptr<Interaction>& c);
		
		void calcTwoSpheres(	Vector3r c1, Real r1, Vector3r c2, Real r2, Real& penetrationDepth, Vector3r& normal, Vector3r& cp);

	REGISTER_CLASS_NAME(InteractingMyTetrahedron2InteractingMyTetrahedron4InteractionOfMyTetrahedron);
	REGISTER_BASE_CLASS_NAME(InteractionGeometryEngineUnit);

	DEFINE_FUNCTOR_ORDER_2D(InteractingMyTetrahedron,InteractingMyTetrahedron);
};

REGISTER_SERIALIZABLE(InteractingMyTetrahedron2InteractingMyTetrahedron4InteractionOfMyTetrahedron,false);

#endif // INTERACTINGMYTETRAHEDRON2INTERACTINGMYTETRAHEDRON4INTERACTIONOFMYTETRAHEDRON_HPP

