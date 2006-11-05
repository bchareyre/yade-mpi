/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "StiffnessMatrix.hpp"


StiffnessMatrix::StiffnessMatrix() : PhysicalAction()
{
	createIndex();
}



StiffnessMatrix::~StiffnessMatrix()
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

void StiffnessMatrix::reset()
{
	//stiffness = Matrix3r::ZERO; //old style
	stiffness = Vector3r::ZERO;
}


shared_ptr<PhysicalAction> StiffnessMatrix::clone()
{
	return shared_ptr<StiffnessMatrix>(new StiffnessMatrix(*this));
}

