/*************************************************************************
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include <yade/pkg-dem/CapillaryParameters.hpp>

CapillaryParameters::CapillaryParameters()
{
	meniscus = false;
	Vmeniscus = 0;
	CapillaryPressure = 0;
	Fcap = Vector3r(0,0,0);
	Delta1 = 0;
	Delta2 = 0;
	currentIndexes[0]=0;
	currentIndexes[1]=0;
	currentIndexes[2]=0;
	currentIndexes[3]=0;
}


CapillaryParameters::~CapillaryParameters()
{
}

// void CapillaryParameters::postProcessAttributes(bool)
// {
// 
// }


void CapillaryParameters::registerAttributes()
{
	ElasticContactInteraction::registerAttributes();
	REGISTER_ATTRIBUTE(Vmeniscus);
	REGISTER_ATTRIBUTE(CapillaryPressure);
	REGISTER_ATTRIBUTE(Fcap);
	REGISTER_ATTRIBUTE(Delta1);
	REGISTER_ATTRIBUTE(Delta2);
	REGISTER_ATTRIBUTE(meniscus);
}
YADE_PLUGIN();
