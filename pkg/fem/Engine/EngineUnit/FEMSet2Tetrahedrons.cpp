/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "FEMSet2Tetrahedrons.hpp"

#include<yade/pkg-fem/FEMSetParameters.hpp>
#include<yade/pkg-fem/FEMTetrahedronData.hpp>


#include<yade/pkg-common/Tetrahedron.hpp>
#include<yade/core/MetaBody.hpp>


void FEMSet2Tetrahedrons::go(	  const shared_ptr<PhysicalParameters>& ph
				, shared_ptr<GeometricalModel>& 
				, const Body* body)
{
	int tetrahedronGroupMask = YADE_CAST<const FEMSetParameters*>(ph.get())->tetrahedronGroupMask;
	const MetaBody * cb = YADE_CAST<const MetaBody*>(body);
	const shared_ptr<BodyContainer>& bodies = cb->bodies;
	
// FIXME - this copying of data! 
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for(  ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> b = *bi;
		if(b->getGroupMask() & tetrahedronGroupMask )
		{
			Tetrahedron* tet 		= YADE_CAST<Tetrahedron*>        (b->geometricalModel.get());
			FEMTetrahedronData* tetData	= YADE_CAST<FEMTetrahedronData*> (b->physicalParameters.get());
			
			tet->v[0] 			= (*(cb->bodies))[tetData->ids[0]]->physicalParameters->se3.position;
			tet->v[1] 			= (*(cb->bodies))[tetData->ids[1]]->physicalParameters->se3.position;
			tet->v[2] 			= (*(cb->bodies))[tetData->ids[2]]->physicalParameters->se3.position;
			tet->v[3] 			= (*(cb->bodies))[tetData->ids[3]]->physicalParameters->se3.position;
		}
	}
}

YADE_PLUGIN("FEMSet2Tetrahedrons");