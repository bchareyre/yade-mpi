/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre <bruno.chareyre@imag.fr>         *
*  Copyright (C) 2008 by Janek Kozicki <cosurgi@berlios.de>              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "CohesiveFrictionalContactInteraction.hpp"


CohesiveFrictionalContactInteraction::CohesiveFrictionalContactInteraction()
{
	createIndex();
	cohesionDisablesFriction = false;
	cohesionBroken = true;
	fragile = true;
	normalAdhesion = 0;
	shearAdhesion = 0;
	moment_twist = Vector3r(0,0,0);
	moment_bending = Vector3r(0,0,0);

// assign neutral value	
	orientationToContact1 = Quaternionr(1.0,0.0,0.0,0.0);
	orientationToContact2 = Quaternionr(1.0,0.0,0.0,0.0);
	initialOrientation1 = Quaternionr(1.0,0.0,0.0,0.0);
	initialOrientation2 = Quaternionr(1.0,0.0,0.0,0.0);
	twistCreep          = Quaternionr(1.0,0.0,0.0,0.0);
	kr = 0;
	currentContactOrientation = Quaternionr(1.0,0.0,0.0,0.0);
	initialContactOrientation = Quaternionr(1.0,0.0,0.0,0.0);
	initialPosition1=initialPosition2=Vector3r(1,0,0);
}

void CohesiveFrictionalContactInteraction::SetBreakingState()
{
	
	//if (fragile) {
		cohesionBroken = true;
		normalAdhesion = 0;
		shearAdhesion = 0;//}
	
}

CohesiveFrictionalContactInteraction::~CohesiveFrictionalContactInteraction()
{
}

// void CohesiveFrictionalContactInteraction::postProcessAttributes(bool)
// {
// 
// }


YADE_PLUGIN("CohesiveFrictionalContactInteraction");