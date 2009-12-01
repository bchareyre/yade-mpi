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
#include<yade/core/MetaEngine.hpp>
#include<yade/core/InteractingGeometry.hpp>
#include<yade/pkg-common/InteractionGeometryEngineUnit.hpp>

class Body;

class InteractionGeometryMetaEngine :	public MetaEngine2D
					<	
						InteractingGeometry,						// base classe for dispatch
						InteractingGeometry,						// base classe for dispatch
						InteractionGeometryEngineUnit,					// class that provides multivirtual call
						bool ,								// return type
						TYPELIST_6(	  const shared_ptr<InteractingGeometry>&	// arguments
								, const shared_ptr<InteractingGeometry>&
								, const State&
								, const State&
								, const Vector3r& 
								, const shared_ptr<Interaction>&
							  )
						, false								// disable auto symmetry handling
					>
{
	bool alreadyWarnedNoCollider;
	public :
		virtual void action(MetaBody*);
		shared_ptr<Interaction> explicitAction(const shared_ptr<Body>& b1, const shared_ptr<Body>& b2);
		InteractionGeometryMetaEngine(): alreadyWarnedNoCollider(false){}

	REGISTER_CLASS_AND_BASE(InteractionGeometryMetaEngine,MetaEngine2D);
	REGISTER_ATTRIBUTES(MetaEngine,/* no attributes here*/ );
	DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(InteractionGeometryMetaEngine);


