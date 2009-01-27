/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/InteractionGeometryEngineUnit.hpp>

/*! 
 * \brief This class exists for the sole purpose of creating InteractionOfMyTetrahedron
 * from colliding InteractingMyTetrahedron with InteractingBox
 *
 * so it must loop through four spheres and calculate for each of them a
 * penetration depth and normal direction of interaction with a box.
 * I will have to fill 4x4 matrix, so I need to emulate that box has four spheres.
 *
*/

class InteractingMyTetrahedron2InteractingBox4InteractionOfMyTetrahedron : public InteractionGeometryEngineUnit
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
		
		void calcSphereAndBox(	Vector3r c1, Real r1, Vector3r& extents, const Se3r& se3Box, Real& penetrationDepth, Vector3r& normal, Vector3r& cp);

	FUNCTOR2D(InteractingMyTetrahedron,InteractingBox);

	REGISTER_CLASS_NAME(InteractingMyTetrahedron2InteractingBox4InteractionOfMyTetrahedron);
	REGISTER_BASE_CLASS_NAME(InteractionGeometryEngineUnit);

	DEFINE_FUNCTOR_ORDER_2D(InteractingMyTetrahedron,InteractingBox);
};

REGISTER_SERIALIZABLE(InteractingMyTetrahedron2InteractingBox4InteractionOfMyTetrahedron);


