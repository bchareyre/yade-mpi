/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#ifndef SPHERE2AABB_HPP
#define SPHERE2AABB_HPP

#include "BoundingVolumeEngineUnit.hpp"

class InteractingSphere2AABB : public BoundingVolumeEngineUnit
{
	public :
		void go(	  const shared_ptr<InteractingGeometry>& cm
				, shared_ptr<BoundingVolume>& bv
				, const Se3r& se3
				, const Body*	);
	REGISTER_CLASS_NAME(InteractingSphere2AABB);
	REGISTER_BASE_CLASS_NAME(BoundingVolumeEngineUnit);
};

REGISTER_SERIALIZABLE(InteractingSphere2AABB,false);

#endif // SPHERE2AABB_HPP

