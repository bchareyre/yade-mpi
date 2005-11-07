/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAWSPHERESHADOWVOLUME_HPP
#define GLDRAWSPHERESHADOWVOLUME_HPP

#include "GLDrawShadowVolumeFunctor.hpp"

class GLDrawSphereShadowVolume : public GLDrawShadowVolumeFunctor
{
	public :
		virtual void go(const shared_ptr<GeometricalModel>&, const shared_ptr<PhysicalParameters>&, const Vector3r&);

	REGISTER_CLASS_NAME(GLDrawSphereShadowVolume);
	REGISTER_BASE_CLASS_NAME(GLDrawShadowVolumeFunctor);
};

REGISTER_SERIALIZABLE(GLDrawSphereShadowVolume,false);

#endif //  GLDRAWSPHERESHADOWVOLUME_HPP

