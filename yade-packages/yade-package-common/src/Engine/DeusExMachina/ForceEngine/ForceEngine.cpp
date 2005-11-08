/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ForceEngine.hpp"
#include "ParticleParameters.hpp"
#include "Force.hpp"


#include <yade/yade-core/MetaBody.hpp>


ForceEngine::ForceEngine() : force(Vector3r::ZERO) , actionParameterForce(new Force)
{
}

ForceEngine::~ForceEngine()
{
}


void ForceEngine::registerAttributes()
{
	REGISTER_ATTRIBUTE(force);
}


void ForceEngine::applyCondition(Body* body)
{
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for( ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> b = *bi;
		static_cast<Force*>( ncb->actionParameters->find( b->getId() , actionParameterForce->getClassIndex() ).get() )->force += force;
        }
}

