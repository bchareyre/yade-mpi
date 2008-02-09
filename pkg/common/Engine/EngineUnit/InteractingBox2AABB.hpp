/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#ifndef INTERACTINGBOX2AABB_HPP
#define INTERACTINGBOX2AABB_HPP


#include "BoundingVolumeEngineUnit.hpp"


class InteractingBox2AABB : public BoundingVolumeEngineUnit
{
	public :
		void go(	  const shared_ptr<InteractingGeometry>& cm
				, shared_ptr<BoundingVolume>& bv
				, const Se3r& se3
				, const Body*	);
	FUNCTOR2D(InteractingBox,AABB);
	REGISTER_CLASS_NAME(InteractingBox2AABB);
	REGISTER_BASE_CLASS_NAME(BoundingVolumeEngineUnit);
};

REGISTER_SERIALIZABLE(InteractingBox2AABB,false);

#endif // INTERACTINGBOX2AABB_HPP

