/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once


#include<yade/core/MetaEngine.hpp>
#include<yade/lib-multimethods/DynLibDispatcher.hpp>
#include<yade/core/Material.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/pkg-common/InteractionPhysicsEngineUnit.hpp>

class InteractionPhysicsMetaEngine : 	public MetaEngine2D
					<	
						Material,					// base classe for dispatch
						Material,					// base classe for dispatch
						InteractionPhysicsEngineUnit,				// class that provides multivirtual call
						void,							// return type
						TYPELIST_3(	  const shared_ptr<Material>&	// arguments
								, const shared_ptr<Material>&
								, const shared_ptr<Interaction>&
							  )
					>
{		
	public :
		virtual void action(MetaBody*);
		void explicitAction(shared_ptr<Material>& pp1, shared_ptr<Material>& pp2, shared_ptr<Interaction>& i);

	REGISTER_CLASS_NAME(InteractionPhysicsMetaEngine);
	REGISTER_BASE_CLASS_NAME(MetaEngine2D);
	REGISTER_ATTRIBUTES(MetaEngine, /* no attributes here */);
};

REGISTER_SERIALIZABLE(InteractionPhysicsMetaEngine);



