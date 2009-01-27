/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawBoxShadowVolume : public GLDrawShadowVolumeFunctor
{
	public : 
		virtual void go(const shared_ptr<GeometricalModel>&, const shared_ptr<PhysicalParameters>&, const Vector3r&);

	RENDERS(Box);
	REGISTER_CLASS_NAME(GLDrawBoxShadowVolume);
	REGISTER_BASE_CLASS_NAME(GLDrawShadowVolumeFunctor);
};

REGISTER_SERIALIZABLE(GLDrawBoxShadowVolume);


