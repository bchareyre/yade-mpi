/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAWBOX_HPP
#define GLDRAWBOX_HPP

#include "GLDrawGeometricalModelFunctor.hpp"

class GLDrawBox : public GLDrawGeometricalModelFunctor
{
	public :
		virtual void go(const shared_ptr<GeometricalModel>&, const shared_ptr<PhysicalParameters>&,bool);

	RENDERS(Box);
	REGISTER_CLASS_NAME(GLDrawBox);
	REGISTER_BASE_CLASS_NAME(GLDrawGeometricalModelFunctor);
};

REGISTER_SERIALIZABLE(GLDrawBox,false);

#endif //  GLDRAWBOX_HPP

