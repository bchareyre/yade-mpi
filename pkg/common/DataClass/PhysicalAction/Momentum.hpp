/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/PhysicalAction.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>

class Momentum : public PhysicalAction
{
	public :
		Vector3r momentum;
		Momentum();
		virtual ~Momentum();

		virtual void reset();
		virtual shared_ptr<PhysicalAction> clone();
	
	REGISTER_CLASS_NAME(Momentum);
	REGISTER_BASE_CLASS_NAME(PhysicalAction);
	virtual void registerAttributes(){REGISTER_ATTRIBUTE(momentum);}
	
	REGISTER_CLASS_INDEX(Momentum,PhysicalAction);
};
REGISTER_SERIALIZABLE(Momentum);


