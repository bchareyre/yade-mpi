/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "RigidBodyParameters.hpp"

RigidBodyParameters::RigidBodyParameters() : ParticleParameters()
{
	createIndex();
	acceleration = Vector3r(0,0,0);
	angularAcceleration = Vector3r(0,0,0);
	angularVelocity=Vector3r(0,0,0);
}

RigidBodyParameters::~RigidBodyParameters()
{
}

void RigidBodyParameters::registerAttributes()
{
	ParticleParameters::registerAttributes();
	REGISTER_ATTRIBUTE(inertia);
	REGISTER_ATTRIBUTE(angularVelocity);
}

YADE_PLUGIN();
