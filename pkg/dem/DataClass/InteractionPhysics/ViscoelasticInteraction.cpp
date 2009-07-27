/*************************************************************************
*  Copyright (C) 2008 by Dorofeenko Sergei                               *
*  sega@users.berlios.de						 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ViscoelasticInteraction.hpp"


ViscoelasticInteraction::ViscoelasticInteraction()
{
	createIndex();
}

ViscoelasticInteraction::~ViscoelasticInteraction()
{
}

YADE_PLUGIN("ViscoelasticInteraction");