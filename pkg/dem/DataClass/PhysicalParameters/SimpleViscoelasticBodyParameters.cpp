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

YADE_PLUGIN((SimpleViscoelasticBodyParameters));
