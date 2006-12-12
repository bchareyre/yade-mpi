/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef ACTIONFORCE_HPP
#define ACTIONFORCE_HPP

#include <yade/yade-core/PhysicalAction.hpp>
#include <Wm3Vector3.h>
#include <yade/yade-lib-base/yadeWm3.hpp>

class Force : public PhysicalAction
{
	public :
		Vector3r force;

		Force();
		virtual ~Force();

/// Methods
//		virtual void add(const shared_ptr<PhysicalAction>& a); // FIXME - not used
//		virtual void sub(const shared_ptr<PhysicalAction>& a); // FIXME - not used

		virtual void reset();
		virtual shared_ptr<PhysicalAction> clone();
		
/// Serialization
	REGISTER_CLASS_NAME(Force);
	REGISTER_BASE_CLASS_NAME(PhysicalAction);

/// Indexable
	REGISTER_CLASS_INDEX(Force,PhysicalAction);
	
};

REGISTER_SERIALIZABLE(Force,false);

#endif // ACTIONFORCE_HPP

