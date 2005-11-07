/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef BOX2SPHERE4SDECCONTACTMODEL_HPP
#define BOX2SPHERE4SDECCONTACTMODEL_HPP

#include <yade/yade-package-common/InteractionGeometryEngineUnit.hpp>

class Box2Sphere4MacroMicroContactGeometry : public InteractionGeometryEngineUnit
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

	REGISTER_CLASS_NAME(Box2Sphere4MacroMicroContactGeometry);
	REGISTER_BASE_CLASS_NAME(InteractionGeometryEngineUnit);

	DEFINE_FUNCTOR_ORDER_2D(InteractingBox,InteractingSphere);
};

REGISTER_SERIALIZABLE(Box2Sphere4MacroMicroContactGeometry,false);

#endif // __BOX2SPHERE4SDECCONTACTMODEL_H__

