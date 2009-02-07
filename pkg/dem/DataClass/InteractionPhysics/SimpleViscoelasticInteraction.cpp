/*************************************************************************
*  Copyright (C) 2008 by Dorofeenko Sergei                               *
*  sega@users.berlios.de						 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "SimpleViscoelasticInteraction.hpp"


SimpleViscoelasticInteraction::SimpleViscoelasticInteraction()
{
	createIndex();
}

SimpleViscoelasticInteraction::~SimpleViscoelasticInteraction()
{
}

void SimpleViscoelasticInteraction::registerAttributes()
{
	REGISTER_ATTRIBUTE(kn);
	REGISTER_ATTRIBUTE(ks);
	REGISTER_ATTRIBUTE(cn);
	REGISTER_ATTRIBUTE(cs);
	REGISTER_ATTRIBUTE(tangensOfFrictionAngle);
	REGISTER_ATTRIBUTE(prevNormal);
	REGISTER_ATTRIBUTE(normalForce);
	REGISTER_ATTRIBUTE(shearForce);
}
YADE_PLUGIN();
