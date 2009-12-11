/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#pragma once
#include<yade/pkg-common/BoundFunctor.hpp>

class DontUseThisClassAnymore_itWillBeRemovedInTheFuture : public BoundFunctor
{
	public :
		void go(	  const shared_ptr<Shape>& cm
				, shared_ptr<Bound>& bv
				, const Se3r& se3
				, const Body* );
	FUNCTOR2D(SceneShape,AABB);
	REGISTER_CLASS_NAME(DontUseThisClassAnymore_itWillBeRemovedInTheFuture);
	REGISTER_BASE_CLASS_NAME(BoundFunctor);
};

REGISTER_SERIALIZABLE(DontUseThisClassAnymore_itWillBeRemovedInTheFuture);

