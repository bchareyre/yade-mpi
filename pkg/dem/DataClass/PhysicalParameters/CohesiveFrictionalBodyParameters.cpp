/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "CohesiveFrictionalBodyParameters.hpp"

CohesiveFrictionalBodyParameters::CohesiveFrictionalBodyParameters () : BodyMacroParameters()
{
	createIndex();
	isBroken=true;
	isCohesive=true;
}

CohesiveFrictionalBodyParameters::~CohesiveFrictionalBodyParameters()
{
}

YADE_PLUGIN((CohesiveFrictionalBodyParameters));
