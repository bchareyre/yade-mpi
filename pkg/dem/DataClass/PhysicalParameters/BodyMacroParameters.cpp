/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "BodyMacroParameters.hpp"

BodyMacroParameters::BodyMacroParameters () : ElasticBodyParameters()
{
	createIndex();
	poisson=.3;
	frictionAngle=0.5236; /* 30˚ */
}

BodyMacroParameters::~BodyMacroParameters()
{
}

YADE_PLUGIN();
