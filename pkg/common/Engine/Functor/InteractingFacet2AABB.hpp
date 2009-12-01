/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#pragma once

#include <yade/pkg-common/BoundingVolumeFunctor.hpp>

class InteractingFacet2AABB : public BoundingVolumeFunctor
{
	public :
		void go(	  const shared_ptr<InteractingGeometry>& cm
				, shared_ptr<BoundingVolume>& bv
				, const Se3r& se3
				, const Body*	);
	
	FUNCTOR2D(InteractingFacet,AABB);
	
	REGISTER_CLASS_NAME(InteractingFacet2AABB);
	REGISTER_BASE_CLASS_NAME(BoundingVolumeFunctor);
};

REGISTER_SERIALIZABLE(InteractingFacet2AABB);


