/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ParticleSetParameters.hpp"


ParticleSetParameters::ParticleSetParameters() : PhysicalParameters()
{
	createIndex();
}


ParticleSetParameters::~ParticleSetParameters()
{
}


YADE_PLUGIN("ParticleSetParameters");