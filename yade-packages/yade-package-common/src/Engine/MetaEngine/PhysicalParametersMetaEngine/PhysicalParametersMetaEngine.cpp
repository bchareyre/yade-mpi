/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "PhysicalParametersMetaEngine.hpp"


#include <yade/yade-core/MetaBody.hpp>


void PhysicalParametersMetaEngine::action(Body* body)
{
	MetaBody * ncb = Dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for( ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> b = *bi;
		operator()(b->physicalParameters,b.get());
	}
	
 	operator()(body->physicalParameters,body);
}

