/*************************************************************************
*  Copyright (C) 2013 by Burak ER                                 	 *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include <pkg/dem/deformablecohesive/DeformableElement.hpp>
#include <pkg/dem/deformablecohesive/FEInternalForceDispatchers.hpp>
YADE_PLUGIN((InternalForceFunctor)(InternalForceDispatcher));
InternalForceFunctor::~InternalForceFunctor(){};


/********************************************************************
                      InternalForceDispatcher
*********************************************************************/

CREATE_LOGGER(InternalForceDispatcher);
void InternalForceDispatcher::action()
{
	//There is no need to make an action for this dispatcher its main misson is to return functors for the shape that is given.
}
//void InternalForceDispatcher::explicitAction(const shared_ptr<DeformableElement> deformableelement,const Body* bdy){
//
//	updateScenePtr();
//	shared_ptr<DeformableElement> deformableElement=deformableelement;
//	if(!deformableElement->functorCache.iff){
//		deformableElement->functorCache.iff=getFunctor1D(deformableElement);
//		if(!deformableElement->functorCache.iff) throw invalid_argument("InternalForceDispatcher::explicitAction did not find a suitable dispatch for type"+deformableElement->getClassName());
//		deformableElement->functorCache.iff->go(deformableElement,bdy);
//	}
//
//
//}

