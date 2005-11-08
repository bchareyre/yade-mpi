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
}

ParticleParameters::~ParticleParameters()
{
}


void ParticleParameters::postProcessAttributes(bool deserializing)
{
	PhysicalParameters::postProcessAttributes(deserializing);
	
	if(deserializing)
	{
		if (mass==0)
			invMass = 0;
		else
			invMass = 1.0/mass;
	}
}


void ParticleParameters::registerAttributes()
{
	PhysicalParameters::registerAttributes();
	//REGISTER_ATTRIBUTE(se3);
	REGISTER_ATTRIBUTE(mass);
	REGISTER_ATTRIBUTE(velocity);
}

