/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractingGeometryMetaEngine.hpp"
#include<yade/core/MetaBody.hpp>

void InteractingGeometryMetaEngine::action(MetaBody* ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for( ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> b = *bi;
		if(b->geometricalModel && b->interactingGeometry)
			operator()(b->geometricalModel,b->interactingGeometry,b->physicalParameters->se3,b.get());
	}
		
	if(ncb->geometricalModel && ncb->interactingGeometry)
		operator()(ncb->geometricalModel,ncb->interactingGeometry,ncb->physicalParameters->se3,ncb);
}

YADE_PLUGIN();
