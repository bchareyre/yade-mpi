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


#include<yade/core/MetaBody.hpp>


ForceEngine::ForceEngine() : actionParameterForce(new Force), force(Vector3r::ZERO)
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


void ForceEngine::applyCondition(MetaBody* ncb)
{
	std::vector<int>::const_iterator ii = subscribedBodies.begin();
	std::vector<int>::const_iterator iiEnd = subscribedBodies.end();
	
	for( ; ii!=iiEnd ; ++ii )
	{
		if(ncb->bodies->exists( *ii ))
		{
			static_cast<Force*>( ncb->physicalActions->find( *ii        , actionParameterForce->getClassIndex() ).get() )->force += force;
		} else {
			std::cerr << "ForceEngine: body " << *ii << "doesn't exist, cannot apply force.";
		}
        }
}

YADE_PLUGIN();
