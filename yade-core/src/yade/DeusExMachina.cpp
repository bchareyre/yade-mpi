/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "DeusExMachina.hpp"

DeusExMachina::DeusExMachina() : Engine()
{
	//startTime = -1;
	//stopTime = -1;
	time = 0;
	subscribedBodies.resize(0);
}

void DeusExMachina::registerAttributes()
{
	REGISTER_ATTRIBUTE(subscribedBodies);
}


void DeusExMachina::action(Body* b)
{
	this->applyCondition(b);
}
