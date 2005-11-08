/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAWPOLYHEDRALSWEPTSPHERE_HPP
#define GLDRAWPOLYHEDRALSWEPTSPHERE_HPP

#include <yade/yade-package-common/GLDrawInteractingGeometryFunctor.hpp>

class GLDrawPolyhedralSweptSphere : public GLDrawInteractingGeometryFunctor
{		
	public :
		virtual void go(const shared_ptr<InteractingGeometry>&, const shared_ptr<PhysicalParameters>&);

	REGISTER_CLASS_NAME(GLDrawPolyhedralSweptSphere);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractingGeometryFunctor);

};

REGISTER_SERIALIZABLE(GLDrawPolyhedralSweptSphere,false);

#endif //  GLDRAWPOLYHEDRALSWEPTSPHERE_HPP

