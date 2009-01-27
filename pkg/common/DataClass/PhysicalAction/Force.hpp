/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/PhysicalAction.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>

class Force : public PhysicalAction
{
	public :
		Vector3r force;

		Force();
		virtual ~Force();

		virtual void reset();
		virtual shared_ptr<PhysicalAction> clone();
		
	REGISTER_CLASS_NAME(Force);
	REGISTER_BASE_CLASS_NAME(PhysicalAction);
	virtual void registerAttributes(){REGISTER_ATTRIBUTE(force);}

	REGISTER_CLASS_INDEX(Force,PhysicalAction);
};

REGISTER_SERIALIZABLE(Force);


