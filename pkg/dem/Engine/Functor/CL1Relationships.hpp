/*************************************************************************
*  Copyright (C) 2008 by Jérôme DURIEZ                                   *
*  duriez@geo.hmg.inpg.fr                                                *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>

/*! \brief The RelationShips for using ContactLaw1

In these RelationShips all the attributes of the interactions (which are of ContactLaw1Interaction type) are computed.
WARNING : as in the others Relationships most of the attributes are computed only once : when the interaction is "new"
 */

class CL1Relationships : public InteractionPhysicsFunctor
{
	public :
		CL1Relationships();

		virtual void go(	const shared_ptr<PhysicalParameters>& b1,
					const shared_ptr<PhysicalParameters>& b2,
					const shared_ptr<Interaction>& interaction);

		Real 		betaR;	// a parameter for computing the maximum value of momentum
					// FIXME : should be better in an other place ?? (like the bodies parameters ?) but for me it was here finally the better place...

		bool		setCohesionNow,
				setCohesionOnNewContacts;
				
		int cohesionDefinitionIteration;
		long iter;//REMOVE THIS

	FUNCTOR2D(CohesiveFrictionalBodyParameters,CohesiveFrictionalBodyParameters);
	REGISTER_CLASS_NAME(CL1Relationships);
	REGISTER_BASE_CLASS_NAME(InteractionPhysicsFunctor);
	REGISTER_ATTRIBUTES(InteractionPhysicsFunctor,(betaR)(setCohesionNow)(setCohesionOnNewContacts));
};

REGISTER_SERIALIZABLE(CL1Relationships);


