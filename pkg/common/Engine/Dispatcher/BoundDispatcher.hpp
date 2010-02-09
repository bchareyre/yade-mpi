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
		virtual void action(Scene*);
		virtual bool isActivated(Scene*){ return activated; }
		shared_ptr<VelocityBins> velocityBins;
	DECLARE_LOGGER;
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(BoundDispatcher,Dispatcher,"Dispatcher for creating/updating Body::bound objects.",
		((bool,activated,true,"Whether the engine is activated (only should be changed by the collider)"))
		((Real,sweepDist,0,"Distance by which enlarge all bounding boxes, to prevent collider from being run at every step (only should be changed by the collider).")),
		/*ctor*/,
		/*py*/YADE_PY_DISPATCHER(BoundDispatcher)
	);
};
REGISTER_SERIALIZABLE(BoundDispatcher);


