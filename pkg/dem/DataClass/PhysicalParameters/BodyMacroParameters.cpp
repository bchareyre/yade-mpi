/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include <yade/pkg-dem/BodyMacroParameters.hpp>

BodyMacroParameters::BodyMacroParameters () : ElasticBodyParameters()
{
	createIndex();
}

BodyMacroParameters::~BodyMacroParameters()
{
}

void BodyMacroParameters::registerAttributes()
{
	ElasticBodyParameters::registerAttributes();
	REGISTER_ATTRIBUTE(poisson);
	REGISTER_ATTRIBUTE(frictionAngle);
}

