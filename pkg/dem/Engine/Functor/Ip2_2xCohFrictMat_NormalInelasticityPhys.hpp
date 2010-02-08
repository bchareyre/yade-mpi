/*************************************************************************
*  Copyright (C) 2008 by Jérôme DURIEZ                                   *
*  duriez@geo.hmg.inpg.fr                                                *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>

/*! \brief The RelationShips for using NormalInelasticityLaw

In these RelationShips all the attributes of the interactions (which are of NormalInelasticityPhys type) are computed.
WARNING : as in the others Relationships most of the attributes are computed only once : when the interaction is "new"
 */

class Ip2_2xCohFrictMat_NormalInelasticityPhys : public InteractionPhysicsFunctor
{
	public :
		Ip2_2xCohFrictMat_NormalInelasticityPhys();

		virtual void go(	const shared_ptr<Material>& b1,
					const shared_ptr<Material>& b2,
					const shared_ptr<Interaction>& interaction);

		Real 		betaR;	// a parameter for computing the maximum value of momentum
					// FIXME : should be better in an other place ?? (like the bodies parameters ?) but for me it was here finally the better place...

		bool		setCohesionNow,
				setCohesionOnNewContacts;
				
		int cohesionDefinitionIteration;
		long iter;//REMOVE THIS

	FUNCTOR2D(CohesiveFrictionalMat,CohesiveFrictionalMat);
	REGISTER_CLASS_NAME(Ip2_2xCohFrictMat_NormalInelasticityPhys);
	REGISTER_BASE_CLASS_NAME(InteractionPhysicsFunctor);
	REGISTER_ATTRIBUTES(InteractionPhysicsFunctor,(betaR)(setCohesionNow)(setCohesionOnNewContacts));
};

REGISTER_SERIALIZABLE(Ip2_2xCohFrictMat_NormalInelasticityPhys);


