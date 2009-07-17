/*************************************************************************
*  Copyright (C) 2008 by Jérôme DURIEZ                                   *
*  duriez@geo.hmg.inpg.fr                                                *
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


YADE_PLUGIN();
