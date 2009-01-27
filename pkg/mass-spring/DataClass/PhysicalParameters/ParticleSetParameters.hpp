/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/PhysicalParameters.hpp>

class ParticleSetParameters : public PhysicalParameters
{
	public :
		ParticleSetParameters();
		virtual ~ParticleSetParameters();

/// Serializable
	REGISTER_CLASS_NAME(ParticleSetParameters);
	REGISTER_BASE_CLASS_NAME(PhysicalParameters);

/// Indexable
	REGISTER_CLASS_INDEX(ParticleSetParameters,PhysicalParameters);
};

REGISTER_SERIALIZABLE(ParticleSetParameters);


