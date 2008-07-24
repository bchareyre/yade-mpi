/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAWAABB_HPP
#define GLDRAWAABB_HPP

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawAABB : public GLDrawBoundingVolumeFunctor
{
	public :
		virtual void go(const shared_ptr<BoundingVolume>&);

	RENDERS(AABB);
	REGISTER_CLASS_NAME(GLDrawAABB);
	REGISTER_BASE_CLASS_NAME(GLDrawBoundingVolumeFunctor);
};

REGISTER_SERIALIZABLE(GLDrawAABB,false);

#endif //  GLDRAWAABB_HPP

