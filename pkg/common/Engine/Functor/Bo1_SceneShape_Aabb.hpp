/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#pragma once
#include<yade/pkg-common/BoundFunctor.hpp>

class DontUseClass_MetaInteractingGeometry2AABB__youCanRemoveItSafelyFromYourSimulation_itWillBeRemovedInTheFutureCompletely : public BoundFunctor
{
	public :
		void go(	  const shared_ptr<Shape>& cm
				, shared_ptr<Bound>& bv
				, const Se3r& se3
				, const Body* );
	FUNCTOR2D(SceneShape,Aabb);
	REGISTER_CLASS_NAME(DontUseClass_MetaInteractingGeometry2AABB__youCanRemoveItSafelyFromYourSimulation_itWillBeRemovedInTheFutureCompletely);
	REGISTER_BASE_CLASS_NAME(BoundFunctor);
};

REGISTER_SERIALIZABLE(DontUseClass_MetaInteractingGeometry2AABB__youCanRemoveItSafelyFromYourSimulation_itWillBeRemovedInTheFutureCompletely);

