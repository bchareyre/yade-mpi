/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "BoundingVolumeMetaEngine.hpp"


#include <yade/yade-core/MetaBody.hpp>


void BoundingVolumeMetaEngine::action(Body* body)
{
	MetaBody * ncb = Dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for( ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> b = *bi;
		if(b->interactingGeometry && b->boundingVolume)
			operator()(b->interactingGeometry,b->boundingVolume,b->physicalParameters->se3,b.get());
	}
		
	operator()(body->interactingGeometry,body->boundingVolume,body->physicalParameters->se3,body);
}



