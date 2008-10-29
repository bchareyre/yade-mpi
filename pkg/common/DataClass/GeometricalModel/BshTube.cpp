/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include<yade/lib-opengl/OpenGLWrapper.hpp>
#include "BshTube.hpp"
#include <iostream>

BshTube::BshTube () : GeometricalModel()
{		
	createIndex();
}


BshTube::~BshTube ()
{		
}


void BshTube::registerAttributes()
{
	GeometricalModel::registerAttributes();
	REGISTER_ATTRIBUTE(radius);
        REGISTER_ATTRIBUTE(half_height);
}

YADE_PLUGIN();
