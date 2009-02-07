/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de						 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "SimpleViscoelasticBodyParameters.hpp"

SimpleViscoelasticBodyParameters::SimpleViscoelasticBodyParameters() : RigidBodyParameters()
{
	createIndex();
}

SimpleViscoelasticBodyParameters::~SimpleViscoelasticBodyParameters()
{
}

void SimpleViscoelasticBodyParameters::registerAttributes()
{
	RigidBodyParameters::registerAttributes();
	REGISTER_ATTRIBUTE(kn);
	REGISTER_ATTRIBUTE(ks);
	REGISTER_ATTRIBUTE(cn);
	REGISTER_ATTRIBUTE(cs);
	REGISTER_ATTRIBUTE(frictionAngle);
}

YADE_PLUGIN();

