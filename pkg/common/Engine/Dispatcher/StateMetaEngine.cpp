/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "StateMetaEngine.hpp"


#include<yade/core/World.hpp>


void StateMetaEngine::action(World* ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	ncb->bex.sync();
	
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for( ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> b = *bi;
		operator()(b->state,b.get(),ncb->bex);
	}
	
 	operator()(ncb->state,ncb,ncb->bex);
}

YADE_REQUIRE_FEATURE(PHYSPAR);
