/*************************************************************************
*  Copyright (C) 2008 by Jérôme DURIEZ                                   *
*  duriez@geo.hmg.inpg.fr                                                *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-dem/FrictPhys.hpp>

/*! \brief Interaction for using NormalInelasticityLaw

This interaction is similar to CohesiveFrictionalContactInteraction. Among the differences are the unMax, previousun and previousFn (allowing to describe the inelastic unloadings in compression), no more shear and tension Adhesion, no more "fragile", "cohesionBroken" and "cohesionDisablesFriction"
 */

class NormalInelasticityPhys : public FrictPhys
{
	public :
		Real		unMax,		// the maximum value of penetration depth of the history of this interaction
				previousun,	// the value of this un at the last time step
				previousFn	// the value of the normal force at the last time step
				,forMaxMoment	// for the maximum value of elastic momentum
				,kr		// rolling stiffness
				;

		Quaternionr	initialOrientation1,initialOrientation2,
				orientationToContact1,orientationToContact2,
				currentContactOrientation,initialContactOrientation;
		Vector3r	initialPosition1,initialPosition2;

	
		NormalInelasticityPhys();
		virtual ~NormalInelasticityPhys();
		void SetBreakingState ();
	REGISTER_ATTRIBUTES(FrictPhys,(unMax)(previousun)(previousFn)(initialOrientation1)(initialOrientation2)(orientationToContact1)(orientationToContact2)(currentContactOrientation)(initialContactOrientation)(initialPosition1)(initialPosition2)(forMaxMoment)(kr));
	REGISTER_CLASS_NAME(NormalInelasticityPhys);
	REGISTER_BASE_CLASS_NAME(FrictPhys);

	REGISTER_CLASS_INDEX(NormalInelasticityPhys,FrictPhys);

};

REGISTER_SERIALIZABLE(NormalInelasticityPhys);


