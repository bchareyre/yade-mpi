/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 

#include "Bo1_SceneShape_Aabb.hpp"


#include<yade/pkg-common/SceneShape.hpp>
#include<yade/pkg-common/Aabb.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>
#include<limits>


void DontUseClass_MetaInteractingGeometry2AABB__youCanRemoveItSafelyFromYourSimulation_itWillBeRemovedInTheFutureCompletely::go(	  const shared_ptr<Shape>&
						, shared_ptr<Bound>& bv
						, const Se3r&
						, const Body* body )
{}
	
YADE_PLUGIN((DontUseClass_MetaInteractingGeometry2AABB__youCanRemoveItSafelyFromYourSimulation_itWillBeRemovedInTheFutureCompletely));
