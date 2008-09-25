/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre                                 *
*  bruno.chareyre@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef COHESIVE_FRICTIONAL_CONTACT_PARAMETERS_MINEBIS_HPP
#define COHESIVE_FRICTIONAL_CONTACT_PARAMETERS_MINEBIS_HPP

#include<yade/pkg-dem/ElasticContactInteraction.hpp>

/*! \brief Interaction for using ContactLaw1

This interaction is similar to CohesiveFrictionalContactInteraction. Among the differences are the unMax, previousun and previousFn (allowing to describe the inelastic unloadings in compression), no more shear and tension Adhesion, no more "fragile", "cohesionBroken" and "cohesionDisablesFriction"
 */

class ContactLaw1Interaction : public ElasticContactInteraction
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

	
		ContactLaw1Interaction();
		virtual ~ContactLaw1Interaction();
		void SetBreakingState ();
	protected :
		virtual void registerAttributes();

	REGISTER_CLASS_NAME(ContactLaw1Interaction);
	REGISTER_BASE_CLASS_NAME(ElasticContactInteraction);

	REGISTER_CLASS_INDEX(ContactLaw1Interaction,ElasticContactInteraction);

};

REGISTER_SERIALIZABLE(ContactLaw1Interaction,false);

#endif // COHESIVE_FRICTIONAL_CONTACT_PARAMETERS_MINEBIS_HPP

