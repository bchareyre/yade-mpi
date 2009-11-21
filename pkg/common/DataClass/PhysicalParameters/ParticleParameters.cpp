/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ParticleParameters.hpp"


ParticleParameters::ParticleParameters() : PhysicalParameters()
{
	createIndex();
	acceleration = Vector3r(0,0,0);
	velocity = Vector3r(0,0,0);
	mass=0;
}

ParticleParameters::~ParticleParameters()
{
}

YADE_PLUGIN((ParticleParameters));

YADE_REQUIRE_FEATURE(PHYSPAR);

