/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@mail.berlios.de                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "SimpleElasticInteraction.hpp"


SimpleElasticInteraction::~SimpleElasticInteraction()
{
}

void SimpleElasticInteraction::registerAttributes()
{
	REGISTER_ATTRIBUTE(kn);
}
