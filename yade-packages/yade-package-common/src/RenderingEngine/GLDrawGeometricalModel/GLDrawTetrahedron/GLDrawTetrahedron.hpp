/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAW_TETRAHEDRON_HPP
#define GLDRAW_TETRAHEDRON_HPP 

#include "GLDrawGeometricalModelFunctor.hpp"

class GLDrawTetrahedron : public GLDrawGeometricalModelFunctor
{
	public :
		virtual void go(const shared_ptr<GeometricalModel>&, const shared_ptr<PhysicalParameters>&,bool);

	RENDERS(Tetrahedron);
	REGISTER_CLASS_NAME(GLDrawTetrahedron);
	REGISTER_BASE_CLASS_NAME(GLDrawGeometricalModelFunctor);
};

REGISTER_SERIALIZABLE(GLDrawTetrahedron,false);

#endif // GLDRAW_TETRAHEDRON_HPP 

