/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include "SpheresContactGeometry.hpp"
YADE_PLUGIN("SpheresContactGeometry");

SpheresContactGeometry::SpheresContactGeometry(): InteractionGeometry(), radius1(0),radius2(0),contactPoint(Vector3r::ZERO){createIndex();}
SpheresContactGeometry::~SpheresContactGeometry(){}
void SpheresContactGeometry::registerAttributes()
{
	REGISTER_ATTRIBUTE(radius1);
	REGISTER_ATTRIBUTE(radius2);
	REGISTER_ATTRIBUTE(contactPoint); // to allow access from python
}

