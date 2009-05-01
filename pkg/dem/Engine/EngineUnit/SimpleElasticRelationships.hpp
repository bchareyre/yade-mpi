/*************************************************************************
*  Copyright (C) 2007 by Bruno CHAREYRE                                 *
*  bruno.chareyre@hmg.inpg.fr                                        *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/InteractionPhysicsEngineUnit.hpp>

class SimpleElasticRelationships : public InteractionPhysicsEngineUnit
{
	public :
		virtual void go(	const shared_ptr<PhysicalParameters>& b1,
					const shared_ptr<PhysicalParameters>& b2,
					const shared_ptr<Interaction>& interaction);
	FUNCTOR2D(BodyMacroParameters,BodyMacroParameters);
	REGISTER_CLASS_AND_BASE(SimpleElasticRelationships,InteractionPhysicsEngineUnit);
	REGISTER_ATTRIBUTES(InteractionPhysicsEngineUnit,/*nothing here*/);
};
REGISTER_SERIALIZABLE(SimpleElasticRelationships);


