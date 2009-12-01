/*************************************************************************
*  Copyright (C) 2007 by Bruno CHAREYRE                                 *
*  bruno.chareyre@hmg.inpg.fr                                        *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>

class SimpleElasticRelationshipsWater : public InteractionPhysicsFunctor
{
	public :
		SimpleElasticRelationshipsWater();

		virtual void go(	const shared_ptr<PhysicalParameters>& b1,
					const shared_ptr<PhysicalParameters>& b2,
					const shared_ptr<Interaction>& interaction);

	REGISTER_ATTRIBUTES(InteractionPhysicsFunctor,/* */);
	FUNCTOR2D(BodyMacroParameters,BodyMacroParameters);
	REGISTER_CLASS_NAME(SimpleElasticRelationshipsWater);
	REGISTER_BASE_CLASS_NAME(InteractionPhysicsFunctor);

};

REGISTER_SERIALIZABLE(SimpleElasticRelationshipsWater);


