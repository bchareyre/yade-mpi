/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#ifndef INTERACTING_MY_TETRAHEDRON_2AABB_HPP
#define INTERACTING_MY_TETRAHEDRON_2AABB_HPP

#include <yade/yade-package-common/BoundingVolumeEngineUnit.hpp>

/*! 
 * \brief This class exists for the sole purpose of creating AABB
 * (AxisAligneBoundingBox) from class InteractingMyTetrahedron
 *
 * AABB is used by class PersistentSAPCollider to detect possible interaction between two
 * bodies. Once you write this class for your InteractingGeometry, interactions
 * will be automatically detected and created for you.
 *
 * See also the explanation in TetrahedronsTest
 *
*/

class InteractingMyTetrahedron2AABB : public BoundingVolumeEngineUnit
{
	public :
		void go(	  const shared_ptr<InteractingGeometry>& cm
				, shared_ptr<BoundingVolume>& bv
				, const Se3r& se3
				, const Body*	);
	REGISTER_CLASS_NAME(InteractingMyTetrahedron2AABB);
	REGISTER_BASE_CLASS_NAME(BoundingVolumeEngineUnit);
};

REGISTER_SERIALIZABLE(InteractingMyTetrahedron2AABB,false);

#endif // INTERACTING_MY_TETRAHEDRON_2AABB_HPP

