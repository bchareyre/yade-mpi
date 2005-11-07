/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef POLYHEDRALSWEPTSPHERE2AABB_HPP
#define POLYHEDRALSWEPTSPHERE2AABB_HPP

#include <yade/yade-package-common/BoundingVolumeEngineUnit.hpp>

class PolyhedralSweptSphere2AABB: public BoundingVolumeEngineUnit
{
	public :
		void go(	  const shared_ptr<InteractingGeometry>& cm
				, shared_ptr<BoundingVolume>& bv
				, const Se3r& se3
				, const Body* body);
	REGISTER_CLASS_NAME(PolyhedralSweptSphere2AABB);
	REGISTER_BASE_CLASS_NAME(BoundingVolumeEngineUnit);
};

REGISTER_SERIALIZABLE(PolyhedralSweptSphere2AABB,false);

#endif // POLYHEDRALSWEPTSPHERE2AABB_HPP


