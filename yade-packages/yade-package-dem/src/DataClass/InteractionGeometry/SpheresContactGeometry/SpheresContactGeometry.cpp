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

// SpheresContactGeometry::SpheresContactGeometry() : InteractionGeometry()
// {
// }
// 
// 
SpheresContactGeometry::~SpheresContactGeometry ()
{
}
// 
// void SpheresContactGeometry::postProcessAttributes(bool)
// {
// 
// }
// 
void SpheresContactGeometry::registerAttributes()
{
	REGISTER_ATTRIBUTE(radius1);
	REGISTER_ATTRIBUTE(radius2);
}

