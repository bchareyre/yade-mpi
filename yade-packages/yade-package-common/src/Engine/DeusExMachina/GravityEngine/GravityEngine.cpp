/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GravityEngine.hpp"
#include "ParticleParameters.hpp"
#include "Force.hpp"
#include <yade/yade-core/MetaBody.hpp>

GravityEngine::GravityEngine() : actionParameterForce(new Force), gravity(Vector3r::ZERO)
{
}


GravityEngine::~GravityEngine()
{
}


void GravityEngine::registerAttributes()
{
	REGISTER_ATTRIBUTE(gravity);
}


void GravityEngine::applyCondition(Body* body)
{
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for( ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> b = *bi;
		ParticleParameters* p = dynamic_cast<ParticleParameters*>(b->physicalParameters.get());
		if (p)
			static_cast<Force*>( ncb->physicalActions->find( b->getId() , actionParameterForce->getClassIndex() ).get() )->force += gravity * p->mass;
        }
}

