/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#pragma once

#include <yade/pkg-common/BoundFunctor.hpp>

class Bo1_Facet_Aabb : public BoundFunctor{
	public:
		void go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body*);
	FUNCTOR2D(Facet,Aabb);
	YADE_CLASS_BASE_DOC_ATTRDECL_CTOR_PY(Bo1_Facet_Aabb,BoundFunctor,"Creates/updates an Aabb of a facet",,,);
};
REGISTER_SERIALIZABLE(Bo1_Facet_Aabb);


