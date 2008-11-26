/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#ifndef METAINTERACTINGGEOMETRY2AABB_HPP
#define METAINTERACTINGGEOMETRY2AABB_HPP

#include<yade/pkg-common/BoundingVolumeEngineUnit.hpp>

class MetaInteractingGeometry2AABB : public BoundingVolumeEngineUnit
{
	public :
		void go(	  const shared_ptr<InteractingGeometry>& cm
				, shared_ptr<BoundingVolume>& bv
				, const Se3r& se3
				, const Body* );
	FUNCTOR2D(MetaInteractingGeometry,AABB);
	REGISTER_CLASS_NAME(MetaInteractingGeometry2AABB);
	REGISTER_BASE_CLASS_NAME(BoundingVolumeEngineUnit);
};

REGISTER_SERIALIZABLE(MetaInteractingGeometry2AABB);

#endif // METAINTERACTINGGEOMETRY2AABB_HPP

