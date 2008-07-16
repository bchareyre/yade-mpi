/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre                                 *
*  bruno.chareyre@imag.fr                                               *
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


void CohesiveFrictionalContactInteraction::registerAttributes()
{
	SimpleElasticInteraction::registerAttributes();
	REGISTER_ATTRIBUTE(prevNormal);
	REGISTER_ATTRIBUTE(shearForce);
	REGISTER_ATTRIBUTE(ks);
	REGISTER_ATTRIBUTE(initialKn);
	REGISTER_ATTRIBUTE(initialKs);
	REGISTER_ATTRIBUTE(tangensOfFrictionAngle);
	REGISTER_ATTRIBUTE(cohesionDisablesFriction);
	REGISTER_ATTRIBUTE(fragile);
	REGISTER_ATTRIBUTE(cohesionBroken);
	REGISTER_ATTRIBUTE(normalAdhesion);
	REGISTER_ATTRIBUTE(shearAdhesion);

// FIXME where this?
	REGISTER_ATTRIBUTE(orientationToContact1);
	REGISTER_ATTRIBUTE(orientationToContact2);
	REGISTER_ATTRIBUTE(initialOrientation1);
	REGISTER_ATTRIBUTE(initialOrientation2);
	REGISTER_ATTRIBUTE(kr);
	REGISTER_ATTRIBUTE(currentContactOrientation);
	REGISTER_ATTRIBUTE(initialContactOrientation);
	REGISTER_ATTRIBUTE(initialPosition1);
	REGISTER_ATTRIBUTE(initialPosition2);
	REGISTER_ATTRIBUTE(twistCreep);
//	REGISTER_ATTRIBUTE(prevX1);
//	REGISTER_ATTRIBUTE(prevX2);
//	REGISTER_ATTRIBUTE(initX1);
//	REGISTER_ATTRIBUTE(initX2);
//		Real		 kn				// normal elastic constant.
//				,ks				// shear elastic constant.
//				,initialKn			// initial normal elastic constant.
//				,initialKs			// initial shear elastic constant.
//				,equilibriumDistance		// equilibrium distance
//				,initialEquilibriumDistance	// initial equilibrium distance
//				,frictionAngle 			// angle of friction, according to Coulumb criterion
//				,tangensOfFrictionAngle;
//	
//		Vector3r	prevNormal			// unit normal of the contact plane.
//				,normalForce			// normal force applied on a DE
//				,shearForce;			// shear force applied on a DE
}
YADE_PLUGIN();
