/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAWCOLLISIONGEOMETRYSET_HPP
#define GLDRAWCOLLISIONGEOMETRYSET_HPP

#include "GLDrawInteractionGeometryFunctor.hpp"

class GLDrawInteractionGeometrySet : public GLDrawInteractionGeometryFunctor
{
	// FIXME : wtf with InteractionGeometrySet !?
	public :
		virtual void go(const shared_ptr<InteractingGeometry>&, const shared_ptr<PhysicalParameters>&);

	REGISTER_CLASS_NAME(GLDrawInteractionGeometrySet);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractionGeometryFunctor);
};

REGISTER_FACTORABLE(GLDrawInteractionGeometrySet);

#endif //  GLDRAWCOLLISIONGEOMETRYSET_HPP


