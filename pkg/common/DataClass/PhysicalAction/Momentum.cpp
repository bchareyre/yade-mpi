/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Momentum.hpp"


Momentum::Momentum() : PhysicalAction()
{
	createIndex();
}

Momentum::~Momentum()
{
}

/* FIXME - not used
void Momentum::add(const shared_ptr<PhysicalAction>& a)
{
	Momentum * m = static_cast<Momentum*>(a.get());
	momentum += m->momentum;
}


void Momentum::sub(const shared_ptr<PhysicalAction>& a)
{
	Momentum * m = static_cast<Momentum*>(a.get());
	momentum -= m->momentum;
}
*/

void Momentum::reset()
{
	momentum = Vector3r::ZERO;
}


shared_ptr<PhysicalAction> Momentum::clone()
{
	return shared_ptr<Momentum>(new Momentum(*this));
}

