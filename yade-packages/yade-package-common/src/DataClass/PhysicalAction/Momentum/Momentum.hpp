/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef MOMENTUM_HPP
#define MOMENTUM_HPP

#include <yade/yade-core/PhysicalAction.hpp>
#include <Wm3Vector3.h>
#include <yade/yade-lib-base/yadeWm3.hpp>

class Momentum : public PhysicalAction
{
	public :
		Vector3r momentum;
		Momentum();
		virtual ~Momentum();

//		virtual void add(const shared_ptr<PhysicalAction>& a); // FIXME - not used
//		virtual void sub(const shared_ptr<PhysicalAction>& a); // FIXME - not used

		virtual void reset();
		virtual shared_ptr<PhysicalAction> clone();
	
/// Serialization
	REGISTER_CLASS_NAME(Momentum);
	REGISTER_BASE_CLASS_NAME(PhysicalAction);
	
/// Indexable
	REGISTER_CLASS_INDEX(Momentum,PhysicalAction);
};

REGISTER_SERIALIZABLE(Momentum,false);

#endif // MOMENTUM_HPP

