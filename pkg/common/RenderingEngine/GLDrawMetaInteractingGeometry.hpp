/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawMetaInteractingGeometry : public GLDrawInteractingGeometryFunctor
{
	public :
		virtual void go(const shared_ptr<Shape>&, const shared_ptr<PhysicalParameters>&,bool,const GLViewInfo&);

	RENDERS(MetaInteractingGeometry);
	REGISTER_CLASS_NAME(GLDrawMetaInteractingGeometry);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractingGeometryFunctor);
};

REGISTER_FACTORABLE(GLDrawMetaInteractingGeometry);



