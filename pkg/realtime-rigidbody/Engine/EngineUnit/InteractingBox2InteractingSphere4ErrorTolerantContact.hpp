/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef BOX2SPHERE4ERRORTOLERANT_HPP
#define BOX2SPHERE4ERRORTOLERANT_HPP

#include<yade/pkg-common/InteractionGeometryEngineUnit.hpp>

class InteractingBox2InteractingSphere4ErrorTolerantContact : public InteractionGeometryEngineUnit
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

	REGISTER_CLASS_NAME(InteractingBox2InteractingSphere4ErrorTolerantContact);
	REGISTER_BASE_CLASS_NAME(InteractionGeometryEngineUnit);
	DEFINE_FUNCTOR_ORDER_2D(Box,Sphere);
};

REGISTER_SERIALIZABLE(InteractingBox2InteractingSphere4ErrorTolerantContact,false);

#endif // __BOX2SPHERE4ERRORTOLERANT_H__

