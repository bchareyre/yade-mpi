/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"ForceEngine.hpp"
#include<yade/pkg-common/ParticleParameters.hpp>
#include<yade/core/MetaBody.hpp>

#include<boost/foreach.hpp>


ForceEngine::ForceEngine() : force(Vector3r::ZERO)
{
}

ForceEngine::~ForceEngine()
{
}


void ForceEngine::registerAttributes()
{
	DeusExMachina::registerAttributes();
	REGISTER_ATTRIBUTE(force);
}


void ForceEngine::applyCondition(MetaBody* ncb){
	FOREACH(body_id_t id, subscribedBodies){
		assert(ncb->bodies->exists(id));
		ncb->bex.addForce(id,force);
	}
}

YADE_PLUGIN();
