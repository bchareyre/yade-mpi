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
// 		Ip2_2xCohFrictMat_NormalInelasticityPhys();

		virtual void go(	const shared_ptr<Material>& b1,
					const shared_ptr<Material>& b2,
					const shared_ptr<Interaction>& interaction);
				
		int cohesionDefinitionIteration; //useful is you want to use setCohesionNow

	FUNCTOR2D(CohesiveFrictionalMat,CohesiveFrictionalMat);
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(Ip2_2xCohFrictMat_NormalInelasticityPhys,
				  InteractionPhysicsFunctor,
				  "The RelationShips for using NormalInelasticityLaw\n \n In these RelationShips all the attributes of the interactions (which are of NormalInelasticityPhys type) are computed. \n WARNING : as in the others Relationships most of the attributes are computed only once : when the interaction is (new)",
				  ((Real,betaR,0.12,"Parameter for computing the torque-stifness : T-stifness = betaR * Rmoy^2"))
				  ((bool,setCohesionNow,false,""))
				  ((bool,setCohesionOnNewContacts,false,"")),
				cohesionDefinitionIteration=-1;
				  );
};

REGISTER_SERIALIZABLE(Ip2_2xCohFrictMat_NormalInelasticityPhys);


