/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#pragma once

#include<yade/pkg-common/BoundingVolumeFunctor.hpp>

#include<yade/core/Dispatcher.hpp>
#include<yade/lib-multimethods/DynLibDispatcher.hpp>
#include<yade/core/InteractingGeometry.hpp>
#include<yade/core/BoundingVolume.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/State.hpp>

class VelocityBins;

class BoundingVolumeDispatcher :	public Dispatcher2D
					<	
						InteractingGeometry,						// base classe for dispatch
						BoundingVolume,							// second base classe for dispatch

						BoundingVolumeFunctor,					// class that provides multivirtual call

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
		//! bounding box will be enlarged by this amount in all 3 dimensions; must be non-negative;
		Real sweepDist;
		BoundingVolumeDispatcher(): activated(true), sweepDist(0) {}
		virtual void action(World*);
		virtual bool isActivated(World*){ return activated; }
		shared_ptr<VelocityBins> velocityBins;
	DECLARE_LOGGER;
	REGISTER_CLASS_NAME(BoundingVolumeDispatcher);
	REGISTER_BASE_CLASS_NAME(Dispatcher2D);
	REGISTER_ATTRIBUTES(Dispatcher,(activated)(sweepDist));
};
REGISTER_SERIALIZABLE(BoundingVolumeDispatcher);


