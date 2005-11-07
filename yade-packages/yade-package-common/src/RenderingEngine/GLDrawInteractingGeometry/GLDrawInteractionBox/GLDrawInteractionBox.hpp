/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAWINTERACTIONBOX_HPP
#define GLDRAWINTERACTIONBOX_HPP

#include "GLDrawInteractionGeometryFunctor.hpp"

class GLDrawInteractionBox : public GLDrawInteractionGeometryFunctor
{
	public :
		virtual void go(const shared_ptr<InteractingGeometry>&, const shared_ptr<PhysicalParameters>&);

	REGISTER_CLASS_NAME(GLDrawInteractionBox);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractionGeometryFunctor);
};

REGISTER_SERIALIZABLE(GLDrawInteractionBox,false);

#endif //  GLDRAWINTERACTIONBOX_HPP

