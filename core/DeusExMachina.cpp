/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "DeusExMachina.hpp"

DeusExMachina::DeusExMachina() : Engine() {
	subscribedBodies.resize(0);
}

void DeusExMachina::registerAttributes() {
	Engine::registerAttributes();
	REGISTER_ATTRIBUTE(subscribedBodies);
}

void DeusExMachina::action(MetaBody* b) {
	this->applyCondition(b);
}
