/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "FEMTetrahedronStiffness.hpp"
#include "FEMSetParameters.hpp"
#include "FEMTetrahedronData.hpp"



// FIXME - uh, libnames FEMTetrahedronData and FEMTetrahedronStiffness are maybe wrong! it will be fixed, when whole tetrahedron-as-interaction mess will be cleaned up

void FEMTetrahedronStiffness::go(	  const shared_ptr<PhysicalParameters>& par
					, Body* body)

{
	MetaBody* rootBody = YADE_CAST<MetaBody*>(body);
	shared_ptr<FEMSetParameters> physics = YADE_PTR_CAST<FEMSetParameters>(par);
	nodeGroupMask = physics->nodeGroupMask;
	tetrahedronGroupMask = physics->tetrahedronGroupMask;
	
	BodyContainer::iterator bi    = rootBody->bodies->begin();
	BodyContainer::iterator biEnd = rootBody->bodies->end();
	for(  ; bi!=biEnd ; ++bi )
	{
	// FIXME - this loop should be somewhere in InteractionPhysicsMetaEngine
		shared_ptr<Body> b = *bi;
		if(b->getGroupMask() & tetrahedronGroupMask)
			YADE_CAST<FEMTetrahedronData*>( b->physicalParameters.get() )->calcKeMatrix(rootBody);
		// FIXME - that should be done inside InteractionPhysicsEngineUnit
	}
	
}
