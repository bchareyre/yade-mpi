/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#pragma once

#include <yade/pkg-common/BoundFunctor.hpp>

class Bo1_Facet_Aabb : public BoundFunctor
{
	public :
		void go(	  const shared_ptr<Shape>& cm
				, shared_ptr<Bound>& bv
				, const Se3r& se3
				, const Body*	);
	
	FUNCTOR2D(Facet,Aabb);
	
	REGISTER_CLASS_NAME(Bo1_Facet_Aabb);
	REGISTER_BASE_CLASS_NAME(BoundFunctor);
};

REGISTER_SERIALIZABLE(Bo1_Facet_Aabb);


