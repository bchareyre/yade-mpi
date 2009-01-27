/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de						 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/InteractionPhysicsEngineUnit.hpp>

/// From viscoelastic parameters of the pair bodys provides
/// viscoelastic parameters of their interaction by the rule of consecutively connection.
class SimpleViscoelasticRelationships : public InteractionPhysicsEngineUnit
{
	public :

		SimpleViscoelasticRelationships();

		virtual void go(	const shared_ptr<PhysicalParameters>& b1,
					const shared_ptr<PhysicalParameters>& b2,
					const shared_ptr<Interaction>& interaction);

	protected :
		virtual void registerAttributes();

	FUNCTOR2D(SimpleViscoelasticBodyParameters,SimpleViscoelasticBodyParameters);
	REGISTER_CLASS_NAME(SimpleViscoelasticRelationships);
	REGISTER_BASE_CLASS_NAME(InteractionPhysicsEngineUnit);

};

REGISTER_SERIALIZABLE(SimpleViscoelasticRelationships);


