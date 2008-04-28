/*************************************************************************
*  Copyright (C) 2008 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "MomentEngine.hpp"
#include "ParticleParameters.hpp"
#include "Momentum.hpp"


#include<yade/core/MetaBody.hpp>


MomentEngine::MomentEngine() : actionParameterMoment(new Momentum), moment(Vector3r::ZERO)
{
}

MomentEngine::~MomentEngine()
{
}


void MomentEngine::registerAttributes()
{
	DeusExMachina::registerAttributes();
	REGISTER_ATTRIBUTE(moment);
}


void MomentEngine::applyCondition(MetaBody* ncb)
{
	
	std::vector<int>::const_iterator ii = subscribedBodies.begin();
	std::vector<int>::const_iterator iiEnd = subscribedBodies.end();
	
	for( ; ii!=iiEnd ; ++ii )
	{
		if(ncb->bodies->exists( *ii ))
		{
			static_cast<Momentum*>( ncb->physicalActions->find( *ii        , actionParameterMoment->getClassIndex() ).get() )->momentum += moment;
		} else {
			std::cerr << "MomentEngine: body " << *ii << "doesn't exist, cannot apply moment.";
		}
        }
}

YADE_PLUGIN();
