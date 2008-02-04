/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Force.hpp"


Force::Force() : PhysicalAction()
{
	createIndex();
}

Force::~Force()
{
}


/* FIXME - not used
void Force::add(const shared_ptr<PhysicalAction>& a)
{
	Force * f = static_cast<Force*>(a.get());
	force += f->force;
}


void Force::sub(const shared_ptr<PhysicalAction>& a)
{
	Force * f = static_cast<Force*>(a.get());
	force -= f->force;
}
*/

void Force::reset()
{
	force = Vector3r::ZERO;
}


shared_ptr<PhysicalAction> Force::clone()
{
	return shared_ptr<Force>(new Force(*this));
}

YADE_PLUGIN();
