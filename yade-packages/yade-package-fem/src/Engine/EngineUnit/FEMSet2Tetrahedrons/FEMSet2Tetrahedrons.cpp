/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "FEMSet2Tetrahedrons.hpp"

#include "FEMSetParameters.hpp"
#include "FEMTetrahedronData.hpp"


#include <yade/yade-package-common/Tetrahedron.hpp>
#include <yade/yade-core/MetaBody.hpp>


void FEMSet2Tetrahedrons::go(	  const shared_ptr<PhysicalParameters>& ph
				, shared_ptr<GeometricalModel>& 
				, const Body* body)
{
	int tetrahedronGroupMask = Dynamic_cast<const FEMSetParameters*>(ph.get())->tetrahedronGroupMask;
	const MetaBody * cb = Dynamic_cast<const MetaBody*>(body);
	const shared_ptr<BodyContainer>& bodies = cb->bodies;
	
// FIXME - this copying of data! 
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for(  ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> b = *bi;
		if(b->getGroupMask() & tetrahedronGroupMask )
		{
			Tetrahedron* tet 		= Dynamic_cast<Tetrahedron*>        (b->geometricalModel.get());
			FEMTetrahedronData* tetData	= Dynamic_cast<FEMTetrahedronData*> (b->physicalParameters.get());
			
			tet->v1 			= (*(cb->bodies))[tetData->ids[0]]->physicalParameters->se3.position;
			tet->v2 			= (*(cb->bodies))[tetData->ids[1]]->physicalParameters->se3.position;
			tet->v3 			= (*(cb->bodies))[tetData->ids[2]]->physicalParameters->se3.position;
			tet->v4 			= (*(cb->bodies))[tetData->ids[3]]->physicalParameters->se3.position;
		}
	}
}

