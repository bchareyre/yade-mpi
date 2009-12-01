/*************************************************************************
*  Copyright (C) 2007 by Bruno CHAREYRE                                 *
*  bruno.chareyre@hmg.inpg.fr                                        *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>

class SimpleElasticRelationships : public InteractionPhysicsFunctor
{
	public :
		virtual void go(	const shared_ptr<Material>& b1,
					const shared_ptr<Material>& b2,
					const shared_ptr<Interaction>& interaction);
	FUNCTOR2D(GranularMat,GranularMat);
	REGISTER_CLASS_AND_BASE(SimpleElasticRelationships,InteractionPhysicsFunctor);
	REGISTER_ATTRIBUTES(InteractionPhysicsFunctor,/*nothing here*/);
};
REGISTER_SERIALIZABLE(SimpleElasticRelationships);


