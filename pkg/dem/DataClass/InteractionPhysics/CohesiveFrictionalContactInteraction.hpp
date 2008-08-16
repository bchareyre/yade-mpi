/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre                                 *
*  bruno.chareyre@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef COHESIVE_FRICTIONAL_CONTACT_PARAMETERS_HPP
#define COHESIVE_FRICTIONAL_CONTACT_PARAMETERS_HPP

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
	
		CohesiveFrictionalContactInteraction();
		virtual ~CohesiveFrictionalContactInteraction();
		void SetBreakingState ();
	protected :
		virtual void registerAttributes();

	REGISTER_CLASS_NAME(CohesiveFrictionalContactInteraction);
	REGISTER_BASE_CLASS_NAME(ElasticContactInteraction);

	REGISTER_CLASS_INDEX(CohesiveFrictionalContactInteraction,ElasticContactInteraction);

};

REGISTER_SERIALIZABLE(CohesiveFrictionalContactInteraction,false);

#endif // COHESIVE_FRICTIONAL_CONTACT_PARAMETERS_HPP

