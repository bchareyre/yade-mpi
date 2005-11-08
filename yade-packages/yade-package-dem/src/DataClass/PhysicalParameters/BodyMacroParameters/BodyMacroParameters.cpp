/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "BodyMacroParameters.hpp"

BodyMacroParameters::BodyMacroParameters () : RigidBodyParameters()
{
	createIndex();
}

BodyMacroParameters::~BodyMacroParameters()
{
}

void BodyMacroParameters::registerAttributes()
{
	RigidBodyParameters::registerAttributes();
	REGISTER_ATTRIBUTE(young);
	REGISTER_ATTRIBUTE(poisson);
	REGISTER_ATTRIBUTE(frictionAngle);
}

