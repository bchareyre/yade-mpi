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
		virtual void go(	const shared_ptr<Material>& b1,
					const shared_ptr<Material>& b2,
					const shared_ptr<Interaction>& interaction);

	FUNCTOR2D(FrictMat,FrictMat);
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(SimpleElasticRelationshipsWater,InteractionPhysicsFunctor, "RelationShips to use CapillaryV+CohesiveLaw\n\n In these RelationShips all the interaction attributes are computed. \n\n.. warning::\n\tas in the others :yref:`Ip2 functors<InteractionPhysicsFunctor>`, most of the attributes are computed only once, when the interaction is new.",,;);
	
};
REGISTER_SERIALIZABLE(SimpleElasticRelationshipsWater);

//// OLD CODE!!!
// class SimpleElasticRelationshipsWater : public InteractionPhysicsFunctor
// {
// 	public :
// 		SimpleElasticRelationshipsWater();
// 
// 		virtual void go(	const shared_ptr<Material>& b1,
// 					const shared_ptr<Material>& b2,
// 					const shared_ptr<Interaction>& interaction);
// 
// 	REGISTER_ATTRIBUTES(InteractionPhysicsFunctor,/* */);
// 	FUNCTOR2D(FrictMat,FrictMat);
// 	REGISTER_CLASS_NAME(SimpleElasticRelationshipsWater);
// 	REGISTER_BASE_CLASS_NAME(InteractionPhysicsFunctor);
// 
// };
// REGISTER_SERIALIZABLE(SimpleElasticRelationshipsWater);