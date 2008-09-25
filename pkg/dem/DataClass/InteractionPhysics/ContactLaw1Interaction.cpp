/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre                                 *
*  bruno.chareyre@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ContactLaw1Interaction.hpp"


ContactLaw1Interaction::ContactLaw1Interaction()
{
	createIndex();

// assign neutral value	
	orientationToContact1 = Quaternionr(1.0,0.0,0.0,0.0);
	orientationToContact2 = Quaternionr(1.0,0.0,0.0,0.0);
	initialOrientation1 = Quaternionr(1.0,0.0,0.0,0.0);
	initialOrientation2 = Quaternionr(1.0,0.0,0.0,0.0);
	kr = 0;
	currentContactOrientation = Quaternionr(1.0,0.0,0.0,0.0);
	initialContactOrientation = Quaternionr(1.0,0.0,0.0,0.0);
	initialPosition1=initialPosition2=Vector3r(1,0,0);
	unMax = 0;
	previousun = 0;
	previousFn = 0;

}

void ContactLaw1Interaction::SetBreakingState()
{
	
	//if (fragile) {

	
}

ContactLaw1Interaction::~ContactLaw1Interaction()
{
}

// void ContactLaw1Interaction::postProcessAttributes(bool)
// {
// 
// }


void ContactLaw1Interaction::registerAttributes()
{
	ElasticContactInteraction::registerAttributes();
	REGISTER_ATTRIBUTE(unMax);
	REGISTER_ATTRIBUTE(previousun);
	REGISTER_ATTRIBUTE(previousFn);
	REGISTER_ATTRIBUTE(initialOrientation1);
	REGISTER_ATTRIBUTE(initialOrientation2);
	REGISTER_ATTRIBUTE(orientationToContact1);
	REGISTER_ATTRIBUTE(orientationToContact2);
	REGISTER_ATTRIBUTE(currentContactOrientation);
	REGISTER_ATTRIBUTE(initialContactOrientation);
	REGISTER_ATTRIBUTE(initialPosition1);
	REGISTER_ATTRIBUTE(initialPosition2);
	REGISTER_ATTRIBUTE(forMaxMoment);
	REGISTER_ATTRIBUTE(kr);
}
YADE_PLUGIN();
