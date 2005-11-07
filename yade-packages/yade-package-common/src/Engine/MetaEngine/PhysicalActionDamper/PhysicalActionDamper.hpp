/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef PHYSICALACTIONDAMPER_HPP
#define PHYSICALACTIONDAMPER_HPP

#include <yade/yade-core/MetaDispatchingEngine2D.hpp>
#include <yade/yade-lib-multimethods/DynLibDispatcher.hpp>
#include <yade/yade-core/PhysicalAction.hpp>
#include "PhysicalActionDamperUnit.hpp"

class Body;

class PhysicalActionDamper : public MetaDispatchingEngine2D
				<	PhysicalAction,						// base classe for dispatch
					PhysicalParameters,					// base classe for dispatch
					PhysicalActionDamperUnit,				// class that provides multivirtual call
					void,							// return type
					TYPELIST_3(	  const shared_ptr<PhysicalAction>&	// function arguments
							, const shared_ptr<PhysicalParameters>& 
							, const Body *
						  )
				>
{
	public :
		virtual void action(Body* body);

	REGISTER_CLASS_NAME(PhysicalActionDamper);
	REGISTER_BASE_CLASS_NAME(MetaDispatchingEngine2D);
};

REGISTER_SERIALIZABLE(PhysicalActionDamper,false);

#endif // __PHYSICALACTIONDAMPER_HPP__

