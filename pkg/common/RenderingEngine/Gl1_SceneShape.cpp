/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Gl1_SceneShape.hpp"


void Gl1_SceneShape::go(const shared_ptr<Shape>& , const shared_ptr<PhysicalParameters>&,bool,const GLViewInfo&)
{


}


YADE_PLUGIN((Gl1_SceneShape));
YADE_REQUIRE_FEATURE(OPENGL)

YADE_REQUIRE_FEATURE(PHYSPAR);

