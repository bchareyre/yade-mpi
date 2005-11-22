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
	count = 0;
	rotation = Vector3r(0,0,0);
//	rotation = Quaternionr(0,0,0,0);
}


LatticeBeamParameters::~LatticeBeamParameters()
{

}

void LatticeBeamParameters::calcStrain() // FIXME - replace that with getStrain(), or strain() (finally private variable makes sense :/ )
{
	strain = (length - initialLength) / initialLength; 
}

void LatticeBeamParameters::registerAttributes()
{
	PhysicalParameters::registerAttributes();
	REGISTER_ATTRIBUTE(id1);
	REGISTER_ATTRIBUTE(id2);
	
	REGISTER_ATTRIBUTE(initialLength);
	REGISTER_ATTRIBUTE(length); // FIXME - can be calculated after deserialization
	
	REGISTER_ATTRIBUTE(initialDirection);
	REGISTER_ATTRIBUTE(direction); // FIXME - can be calculated after deserialization
	
//	REGISTER_ATTRIBUTE(initialAngles);
//	REGISTER_ATTRIBUTE(angles); // FIXME - can be calculated after deserialization
	
	REGISTER_ATTRIBUTE(criticalTensileStrain);
	REGISTER_ATTRIBUTE(criticalCompressiveStrain);
	
	REGISTER_ATTRIBUTE(longitudalStiffness);
	REGISTER_ATTRIBUTE(bendingStiffness);
	
	REGISTER_ATTRIBUTE(se3Displacement)
}

void LatticeBeamParameters::postProcessAttributes(bool d)
{
	if(d)
		this->calcStrain();
}

