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
	cohesionBroken = false;
	fragile = true;
	normalAdhesion = 0;
	shearAdhesion = 0;
	
}

void CohesiveFrictionalContactInteraction::SetBreakingState()
{
	
	if (fragile) {
		cohesionBroken = true;
		normalAdhesion = 0;
		shearAdhesion = 0;}
	
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
	REGISTER_ATTRIBUTE(initialKn);
	REGISTER_ATTRIBUTE(initialKs);
	REGISTER_ATTRIBUTE(tangensOfFrictionAngle);
	REGISTER_ATTRIBUTE(cohesionDisablesFriction);
	REGISTER_ATTRIBUTE(fragile);
	REGISTER_ATTRIBUTE(cohesionBroken);
	REGISTER_ATTRIBUTE(normalAdhesion);
	REGISTER_ATTRIBUTE(shearAdhesion);
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
