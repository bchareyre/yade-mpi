/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "LatticeBeamAngularSpring.hpp"


LatticeBeamAngularSpring::LatticeBeamAngularSpring() : InteractionPhysics()
{
//	createIndex();
}


LatticeBeamAngularSpring::~LatticeBeamAngularSpring()
{

}

void LatticeBeamAngularSpring::registerAttributes()
{
	InteractionPhysics::registerAttributes();
	REGISTER_ATTRIBUTE(initialAngle);
	REGISTER_ATTRIBUTE(angle); // FIXME - can be calculated after deserialization
}

