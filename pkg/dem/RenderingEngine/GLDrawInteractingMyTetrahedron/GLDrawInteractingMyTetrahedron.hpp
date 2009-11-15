/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/GLDrawFunctors.hpp>

/*! 
 * \brief This class exists for the sole purpose of drawing InteractingMyTetrahedron using OpenGL
*/

class GLDrawInteractingMyTetrahedron : public GLDrawInteractingGeometryFunctor
{	
	public :
		virtual void go(const shared_ptr<InteractingGeometry>&, const shared_ptr<PhysicalParameters>&,bool,const GLViewInfo&);

	RENDERS(InteractingMyTetrahedron);
	REGISTER_CLASS_NAME(GLDrawInteractingMyTetrahedron);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractingGeometryFunctor);
};

REGISTER_SERIALIZABLE(GLDrawInteractingMyTetrahedron);


