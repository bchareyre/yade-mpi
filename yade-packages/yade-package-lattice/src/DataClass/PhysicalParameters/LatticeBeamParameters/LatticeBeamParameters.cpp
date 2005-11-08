/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "LatticeBeamParameters.hpp"


LatticeBeamParameters::LatticeBeamParameters() : RigidBodyParameters()
{
	createIndex();
}


LatticeBeamParameters::~LatticeBeamParameters()
{

}


void LatticeBeamParameters::registerAttributes()
{
	RigidBodyParameters::registerAttributes();
	REGISTER_ATTRIBUTE(id1);
	REGISTER_ATTRIBUTE(id2);
	REGISTER_ATTRIBUTE(initialLength);
	REGISTER_ATTRIBUTE(length);
}
