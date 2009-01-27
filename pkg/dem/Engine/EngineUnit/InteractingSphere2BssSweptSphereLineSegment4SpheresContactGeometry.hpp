/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/InteractionGeometryEngineUnit.hpp>

class InteractingSphere2BssSweptSphereLineSegment4SpheresContactGeometry : public InteractionGeometryEngineUnit
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

        REGISTER_CLASS_NAME(InteractingSphere2BssSweptSphereLineSegment4SpheresContactGeometry);
	REGISTER_BASE_CLASS_NAME(InteractionGeometryEngineUnit);

        FUNCTOR2D(InteractingSphere,BssSweptSphereLineSegment);

        DEFINE_FUNCTOR_ORDER_2D(InteractingSphere,BssSweptSphereLineSegment);
};

REGISTER_SERIALIZABLE(InteractingSphere2BssSweptSphereLineSegment4SpheresContactGeometry);


