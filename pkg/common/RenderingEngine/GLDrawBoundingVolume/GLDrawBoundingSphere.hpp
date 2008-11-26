/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAWBOUNDINGSPHERE_HPP
#define GLDRAWBOUNDINGSPHERE_HPP

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawBoundingSphere : public GLDrawBoundingVolumeFunctor
{	
	public :
		virtual void go(const shared_ptr<BoundingVolume>&);

	RENDERS(BoundingSphere);
	REGISTER_CLASS_NAME(GLDrawBoundingSphere);	
	REGISTER_BASE_CLASS_NAME(GLDrawBoundingVolumeFunctor);
};

REGISTER_SERIALIZABLE(GLDrawBoundingSphere);

#endif //  GLDRAWBOUNDINGSPHERE_HPP

