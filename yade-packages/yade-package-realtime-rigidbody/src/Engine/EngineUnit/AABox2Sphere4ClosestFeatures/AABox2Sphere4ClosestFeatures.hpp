/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#ifndef AABOX2SPHERE4CLOSESTFEATURES_HPP
#define AABOX2SPHERE4CLOSESTFEATURES_HPP


#include <yade/yade-package-common/InteractionGeometryEngineUnit.hpp>


/*! \brief Provide interaction handling between a axis aligned box and a sphere in terms of ClosestFeatures */

class AABox2Sphere4ClosestFeatures : public InteractionGeometryEngineUnit
{
	public : 
		virtual bool go(		const shared_ptr<InteractingGeometry>& cm1,
						const shared_ptr<InteractingGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<InteractingGeometry>& cm1,
						const shared_ptr<InteractingGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						const shared_ptr<Interaction>& c);

	REGISTER_CLASS_NAME(AABox2Sphere4ClosestFeatures);
	REGISTER_BASE_CLASS_NAME(InteractionGeometryEngineUnit);

	DEFINE_FUNCTOR_ORDER_2D(InteractingBox,InteractingSphere);
	
};

REGISTER_SERIALIZABLE(AABox2Sphere4ClosestFeatures,false);

#endif // AABOX2SPHERE4CLOSESTFEATURES_HPP

