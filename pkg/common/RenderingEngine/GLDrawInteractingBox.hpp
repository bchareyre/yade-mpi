/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawInteractingBox : public GLDrawInteractingGeometryFunctor
{
	public :
		virtual void go(const shared_ptr<InteractingGeometry>&, const shared_ptr<State>&,bool,const GLViewInfo&);

	RENDERS(InteractingBox);
	REGISTER_CLASS_NAME(GLDrawInteractingBox);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractingGeometryFunctor);
};

REGISTER_SERIALIZABLE(GLDrawInteractingBox);


