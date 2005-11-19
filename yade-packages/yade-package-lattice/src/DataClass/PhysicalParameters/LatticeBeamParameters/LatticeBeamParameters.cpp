/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "LatticeBeamParameters.hpp"


LatticeBeamParameters::LatticeBeamParameters() : PhysicalParameters()
{
	createIndex();
}


LatticeBeamParameters::~LatticeBeamParameters()
{

}

void LatticeBeamParameters::calcStrain()
{
	strain = (length - initialLength) / initialLength; 
}

void LatticeBeamParameters::registerAttributes()
{
	PhysicalParameters::registerAttributes();
	REGISTER_ATTRIBUTE(id1);
	REGISTER_ATTRIBUTE(id2);
	
	REGISTER_ATTRIBUTE(initialLength);
	REGISTER_ATTRIBUTE(length);
	
	REGISTER_ATTRIBUTE(direction);
	REGISTER_ATTRIBUTE(initialDirection);
	
	REGISTER_ATTRIBUTE(criticalTensileStrain);
	REGISTER_ATTRIBUTE(criticalCompressiveStrain);
	
	REGISTER_ATTRIBUTE(longitudalStiffness);
	REGISTER_ATTRIBUTE(bendingStiffness);
	
	REGISTER_ATTRIBUTE(previousSe3)
}

void LatticeBeamParameters::postProcessAttributes(bool d)
{
	if(d)
		this->calcStrain();
}

