/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@mail.berlios.de                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/InteractionPhysicsEngineUnit.hpp>

class ElasticBodySimpleRelationship : public InteractionPhysicsEngineUnit
{
	public :
		virtual void go(	const shared_ptr<PhysicalParameters>& b1,
					const shared_ptr<PhysicalParameters>& b2,
					const shared_ptr<Interaction>& interaction);

	protected :
		virtual void registerAttributes();

	FUNCTOR2D(ElasticBodyParameters,ElasticBodyParameters);

	REGISTER_CLASS_NAME(ElasticBodySimpleRelationship);
	REGISTER_BASE_CLASS_NAME(InteractionPhysicsEngineUnit);

};

REGISTER_SERIALIZABLE(ElasticBodySimpleRelationship);


