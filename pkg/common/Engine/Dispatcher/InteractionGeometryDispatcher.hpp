/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <vector>
#include <list>

#include<yade/core/Interaction.hpp>
#include<yade/lib-multimethods/DynLibDispatcher.hpp>
#include<yade/core/Dispatcher.hpp>
#include<yade/core/Shape.hpp>
#include<yade/pkg-common/InteractionGeometryFunctor.hpp>

class Body;

class InteractionGeometryDispatcher :	public Dispatcher2D
					<	
						Shape,						// base classe for dispatch
						Shape,						// base classe for dispatch
						InteractionGeometryFunctor,					// class that provides multivirtual call
						bool ,								// return type
						TYPELIST_7(	  const shared_ptr<Shape>&	// arguments
								, const shared_ptr<Shape>&
								, const State&
								, const State&
								, const Vector3r& 
								, const bool &
								, const shared_ptr<Interaction>&
							  )
						, false								// disable auto symmetry handling
					>
{
	bool alreadyWarnedNoCollider;
	public :
		virtual void action(Scene*);
		shared_ptr<Interaction> explicitAction(const shared_ptr<Body>& b1, const shared_ptr<Body>& b2, bool force);
		InteractionGeometryDispatcher(): alreadyWarnedNoCollider(false){}

	REGISTER_CLASS_AND_BASE(InteractionGeometryDispatcher,Dispatcher2D);
	REGISTER_ATTRIBUTES(Dispatcher,/* no attributes here*/ );
	DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(InteractionGeometryDispatcher);


