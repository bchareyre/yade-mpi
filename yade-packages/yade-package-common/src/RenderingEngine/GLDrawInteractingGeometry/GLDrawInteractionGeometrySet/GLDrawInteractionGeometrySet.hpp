/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAWCOLLISIONGEOMETRYSET_HPP
#define GLDRAWCOLLISIONGEOMETRYSET_HPP

#include "GLDrawInteractingGeometryFunctor.hpp"

class GLDrawMetaInteractingGeometry : public GLDrawInteractingGeometryFunctor
{
	// FIXME : wtf with InteractionGeometrySet !?
	public :
		virtual void go(const shared_ptr<InteractingGeometry>&, const shared_ptr<PhysicalParameters>&);

	REGISTER_CLASS_NAME(GLDrawMetaInteractingGeometry);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractingGeometryFunctor);
};

REGISTER_FACTORABLE(GLDrawMetaInteractingGeometry);

#endif //  GLDRAWCOLLISIONGEOMETRYSET_HPP


