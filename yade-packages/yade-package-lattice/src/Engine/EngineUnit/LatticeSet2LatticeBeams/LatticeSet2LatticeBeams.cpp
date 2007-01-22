/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "LatticeSet2LatticeBeams.hpp"
#include "LatticeSetParameters.hpp"
#include "LatticeBeamParameters.hpp"
#include "LineSegment.hpp"
#include <yade/yade-core/MetaBody.hpp>

void LatticeSet2LatticeBeams::go(	  const shared_ptr<PhysicalParameters>& ph
					, shared_ptr<GeometricalModel>& 
					, const Body* body)
{
	int beamGroupMask = static_cast<const LatticeSetParameters*>(ph.get())->beamGroupMask;
	const MetaBody * ncb = static_cast<const MetaBody*>(body);
	const shared_ptr<BodyContainer>& bodies = ncb->bodies;

	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for(  ; bi!=biEnd ; ++bi )
	{
		Body* b = (*bi).get();
		if( b->getGroupMask() & beamGroupMask )
		{
			LineSegment* line 		= static_cast<LineSegment*>	     (b->geometricalModel.get());
			LatticeBeamParameters* beam 	= static_cast<LatticeBeamParameters*>(b->physicalParameters.get());

// FIXME - this copying of length between latticeBeam geometry and physics, inside MetaBody could
//         be done just once, if length was inside shared_ptr. This can be improved once we make
//         indexable Parameters: Velocity, Position, Orientation, ....
			line->length                    = beam->length;

			// FIXME - display aggregates as brown, bonds as dark brown.
			      if(beam->criticalTensileStrain > 0.00015) line->diffuseColor = Vector3f(0.6,0.6,0.6); else // CEMENT
			      if(beam->criticalTensileStrain > 0.00006) line->diffuseColor = Vector3f(0.0,0.0,0.0); else // AGGREGATE
			                                                line->diffuseColor = Vector3f(0.3,0.3,0.3);      // BOND

			      if(beam->longitudalStiffness > 3.6 ) line->diffuseColor = Vector3f(0.4,0.4,1.0); // NON-DESTROY
		}
	}
}

