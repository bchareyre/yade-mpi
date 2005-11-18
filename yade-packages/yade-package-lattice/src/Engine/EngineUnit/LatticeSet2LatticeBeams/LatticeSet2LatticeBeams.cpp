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

	static Real maxTensileFactor = 0.0; // FIXME - thread unsafe
	static Real maxCompressFactor = 0.0; // FIXME - thread unsafe

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
			Real strain 			= (beam->length - beam->initialLength) / beam->initialLength;
			Real factor;
			if( strain > 0 ) // FIXME - strain is a property of beam
				factor 			= strain / beam->criticalTensileStrain; // positive
			else
				factor 			= strain / beam->criticalCompressiveStrain; // negative
			
			} // compute optimal red/blue colors	
				maxTensileFactor		= std::max(factor,maxTensileFactor);
				maxCompressFactor 		= std::min(factor,maxCompressFactor);
				if(factor > 0 && maxTensileFactor > 0)
				{
					factor 			/= maxTensileFactor;
					line->diffuseColor 	= Vector3f(0.9,0.9,1.0) - factor * Vector3f(0.9,0.9,0.0);
				}
				else if (factor < 0 && maxCompressFactor < 0)
				{
					factor 			/= maxCompressFactor;
					line->diffuseColor 	= Vector3f(1.0,0.9,0.9) - factor * Vector3f(0.0,0.9,0.9);
				}
				else
					line->diffuseColor 	= Vector3f(0.9,0.9,0.9);
			}
		
			line->length       		= beam->length;
		}
	}
}

