/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre <bruno.chareyre@imag.fr>         *
*  Copyright (C) 2008 by Janek Kozicki <cosurgi@berlios.de>              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/NormalShearInteractions.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>

class CohesiveFrictionalContactInteraction : public ElasticContactInteraction
{
	public :
		bool		cohesionDisablesFriction,	//is shear strength the sum of friction and adhesion or only adhesion?
				cohesionBroken,	//is cohesion active? should be set to false when a fragile contact is broken
				fragile;	//do cohesion disapear when contact strength is exceeded?

		Real		normalAdhesion			// max tensile force
				,shearAdhesion;			// max shear force (actual max force can include friction too depending on cohesionDisablesFriction)

		Quaternionr	initialOrientation1,initialOrientation2,
				orientationToContact1,orientationToContact2,
				currentContactOrientation,initialContactOrientation,
				twistCreep;
		Vector3r	initialPosition1,initialPosition2;
		Real		kr; // rolling stiffness
		Vector3r	moment_twist,moment_bending;
	
		CohesiveFrictionalContactInteraction();
		virtual ~CohesiveFrictionalContactInteraction();
		void SetBreakingState ();

	REGISTER_ATTRIBUTES(NormalInteraction,
		(prevNormal)(shearForce)(ks)(initialKn)(initialKs)(tangensOfFrictionAngle)(cohesionDisablesFriction)(fragile)(cohesionBroken)(normalAdhesion)(shearAdhesion)
		// FIXME where this?
		(orientationToContact1)(orientationToContact2)(initialOrientation1)(initialOrientation2)(kr)(currentContactOrientation)(initialContactOrientation)(initialPosition1)(initialPosition2)(twistCreep)
		(moment_twist)(moment_bending)
	);
	REGISTER_CLASS_NAME(CohesiveFrictionalContactInteraction);
	REGISTER_BASE_CLASS_NAME(ElasticContactInteraction);
	REGISTER_CLASS_INDEX(CohesiveFrictionalContactInteraction,ElasticContactInteraction);

};

REGISTER_SERIALIZABLE(CohesiveFrictionalContactInteraction);

