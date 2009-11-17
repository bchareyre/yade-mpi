/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GeometricalModelMetaEngine.hpp"
#include<yade/core/MetaBody.hpp>
YADE_REQUIRE_FEATURE(shape);

void GeometricalModelMetaEngine::action(MetaBody* ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for( ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> b = *bi;
		if(b->geometricalModel)
			operator()(b->physicalParameters,b->geometricalModel,b.get());
	}
	
	if(ncb->geometricalModel)
	 	operator()(ncb->physicalParameters,ncb->geometricalModel,ncb);
}

YADE_PLUGIN((GeometricalModelMetaEngine));
