/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SPHERE2SPHERE4CLOSESTFEATURES_HPP
#define SPHERE2SPHERE4CLOSESTFEATURES_HPP


#include<yade/pkg-common/InteractionGeometryEngineUnit.hpp>

class InteractingSphere2InteractingSphere4ClosestFeatures : public InteractionGeometryEngineUnit
{
	public :
		virtual bool go(	const shared_ptr<InteractingGeometry>& cm1,
					const shared_ptr<InteractingGeometry>& cm2,
					const Se3r& se31,
					const Se3r& se32,
					const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<InteractingGeometry>& cm1,
					const shared_ptr<InteractingGeometry>& cm2,
					const Se3r& se31,
					const Se3r& se32,
					const shared_ptr<Interaction>& c);
	
	FUNCTOR2D(InteractingSphere,InteractingSphere);
	REGISTER_CLASS_NAME(InteractingSphere2InteractingSphere4ClosestFeatures);
	REGISTER_BASE_CLASS_NAME(InteractionGeometryEngineUnit);
	DEFINE_FUNCTOR_ORDER_2D(InteractingSphere,InteractingSphere);
};

REGISTER_SERIALIZABLE(InteractingSphere2InteractingSphere4ClosestFeatures);

#endif // SPHERE2SPHERE4CLOSESTFEATURES_HPP

