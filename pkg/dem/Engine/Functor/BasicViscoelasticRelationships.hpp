/*************************************************************************
*  Copyright (C) 2009 by Vincent Richefeu				 *
*  Vincent.Richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>

/// Determine elastic stiffnesses and viscous parameters from elastic and dimensionless viscoelastic parameters 
/// of the interacting bodies 
class BasicViscoelasticRelationships : public InteractionPhysicsFunctor
{
	public :

		BasicViscoelasticRelationships();

		virtual void go(	const shared_ptr<PhysicalParameters>& b1,
					const shared_ptr<PhysicalParameters>& b2,
					const shared_ptr<Interaction>& interaction);

	REGISTER_ATTRIBUTES(InteractionPhysicsFunctor,/* */);
	FUNCTOR2D(SimpleViscoelasticBodyParameters,SimpleViscoelasticBodyParameters);
	REGISTER_CLASS_NAME(BasicViscoelasticRelationships);
	REGISTER_BASE_CLASS_NAME(InteractionPhysicsFunctor);
};

REGISTER_SERIALIZABLE(BasicViscoelasticRelationships);


