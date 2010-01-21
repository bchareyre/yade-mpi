/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#pragma once

#include<yade/pkg-common/BoundFunctor.hpp>

#include<yade/core/Dispatcher.hpp>
#include<yade/lib-multimethods/DynLibDispatcher.hpp>
#include<yade/core/Shape.hpp>
#include<yade/core/Bound.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/State.hpp>

class VelocityBins;

class BoundDispatcher :	public Dispatcher2D
					<	
						Shape,						// base classe for dispatch
						Bound,							// second base classe for dispatch

						BoundFunctor,					// class that provides multivirtual call

						void,								// return type
						TYPELIST_4(	  const shared_ptr<Shape>&	// arguments
								, shared_ptr<Bound>&
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
		BoundDispatcher(): activated(true), sweepDist(0) {}
		virtual void action(Scene*);
		virtual bool isActivated(Scene*){ return activated; }
		shared_ptr<VelocityBins> velocityBins;
	DECLARE_LOGGER;
	YADE_CLASS_BASE_ATTRS_PY(BoundDispatcher,Dispatcher,(activated)(sweepDist),
		YADE_PY_DISPATCHER(BoundDispatcher)
	);
};
REGISTER_SERIALIZABLE(BoundDispatcher);


