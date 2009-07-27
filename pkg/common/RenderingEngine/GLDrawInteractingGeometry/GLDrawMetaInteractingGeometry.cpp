/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLDrawMetaInteractingGeometry.hpp"


void GLDrawMetaInteractingGeometry::go(const shared_ptr<InteractingGeometry>& , const shared_ptr<PhysicalParameters>&,bool)
{


}


YADE_PLUGIN("GLDrawMetaInteractingGeometry");
YADE_REQUIRE_FEATURE(OPENGL)
