/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg/common/GLDrawFunctors.hpp>
#include<yade/pkg/common/Aabb.hpp>

class Gl1_Aabb: public GlBoundFunctor{
	public:
		virtual void go(const shared_ptr<Bound>&, Scene*);
	RENDERS(Aabb);
	YADE_CLASS_BASE_DOC(Gl1_Aabb,GlBoundFunctor,"Render Axis-aligned bounding box (:yref:`Aabb`).");
};
REGISTER_SERIALIZABLE(Gl1_Aabb);


