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


#include<yade/core/MetaBody.hpp>


void BoundingVolumeMetaEngine::action(MetaBody* ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	const long numBodies=(long)bodies->size();
	//#pragma omp parallel for
	for(int id=0; id<numBodies; id++){
		if(bodies->exists(id)) // don't delete this check  - Janek
		{
			const shared_ptr<Body>& b=(*bodies)[id];
			if(b->interactingGeometry && b->boundingVolume) operator()(b->interactingGeometry,b->boundingVolume,b->physicalParameters->se3,b.get());
		}
	}
	operator()(ncb->interactingGeometry,ncb->boundingVolume,ncb->physicalParameters->se3,ncb);
}



YADE_PLUGIN();
