/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#pragma once

#include<yade/pkg-common/BoundingVolumeEngineUnit.hpp>

#include<yade/core/MetaEngine2D.hpp>
#include<yade/lib-multimethods/DynLibDispatcher.hpp>
#include<yade/core/InteractingGeometry.hpp>
#include<yade/core/BoundingVolume.hpp>
#include<yade/core/Body.hpp>

class BoundingVolumeMetaEngine :	public MetaEngine2D
					<	
						InteractingGeometry,						// base classe for dispatch
						BoundingVolume,							// second base classe for dispatch

						BoundingVolumeEngineUnit,					// class that provides multivirtual call

						void,								// return type
						TYPELIST_4(	  const shared_ptr<InteractingGeometry>&	// arguments
								, shared_ptr<BoundingVolume>&
								, const Se3r&
								, const Body* 					// with that - functors have all the data they may need
							  )
					>
{
	public :
		// selectively turn off this engine (from within the collider, for instance)
		bool activated;
		//! if < 0, will set bounding volumes that the bodies (most likely) will not go over within given time interval; otherwise, do not sweep
		Real sweepTime;
		//! bounding box will be enlarged by this amount in all 3 dimensions;
		/// if negative, it is relative size (e.g. -2 will make the bounding box 2 × bigger)
		/// if positive, it is absolute distance; its will be added in all 6 directions (i.e. dimensions will be enlarged by 2 × this value)
		Real sweepDist;
		BoundingVolumeMetaEngine(): activated(true), sweepTime(-1), sweepDist(0) {}
		virtual void action(MetaBody*);
		virtual bool isActivated(MetaBody*){ return activated; }
	DECLARE_LOGGER;
	REGISTER_CLASS_NAME(BoundingVolumeMetaEngine);
	REGISTER_BASE_CLASS_NAME(MetaEngine2D);
	REGISTER_ATTRIBUTES(MetaEngine,(activated)(sweepTime)(sweepDist));
};

REGISTER_SERIALIZABLE(BoundingVolumeMetaEngine);


