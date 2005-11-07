/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef BOX2BOX4CLOSESTFEATURES_HPP
#define BOX2BOX4CLOSESTFEATURES_HPP

#include <yade/yade-package-common/InteractionGeometryEngineUnit.hpp>

class Box2Box4ClosestFeatures : public InteractionGeometryEngineUnit
{
	private : 
		struct BoxBoxInteractionInfo
		{
			bool isNormalPrincipalAxis;
			bool invertNormal;
			Real penetrationDepth;
			int code;
			Vector3r normal;
		};

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

	private :
		bool testSeparatingAxis(Real expr1, Real expr2, Vector3r n,int c,BoxBoxInteractionInfo* bbInfo);

	REGISTER_CLASS_NAME(Box2Box4ClosestFeatures);
	REGISTER_BASE_CLASS_NAME(InteractionGeometryEngineUnit);
	DEFINE_FUNCTOR_ORDER_2D(InteractingBox,InteractingBox);
};

REGISTER_SERIALIZABLE(Box2Box4ClosestFeatures,false);

#endif // BOX2BOX4CLOSESTFEATURES_HPP

