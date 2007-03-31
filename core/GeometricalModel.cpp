/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GeometricalModel.hpp"

void GeometricalModel::registerAttributes()
{
	REGISTER_ATTRIBUTE(diffuseColor);
	REGISTER_ATTRIBUTE(wire);
	REGISTER_ATTRIBUTE(visible);
	REGISTER_ATTRIBUTE(shadowCaster);
}

