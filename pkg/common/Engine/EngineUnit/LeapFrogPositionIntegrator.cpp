/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "LeapFrogPositionIntegrator.hpp" 
#include "ParticleParameters.hpp"
#include<yade/core/Omega.hpp>

// FIXME : should we pass timestep as parameter of functor
// FIXME : what's with timestepper
void LeapFrogPositionIntegrator::go(       const shared_ptr<PhysicalParameters>& b
						, Body* body)
{
	if(!body->isDynamic) return;

	unsigned int id = body->getId();
	
	if (prevVelocities.size()<=id)
	{
		prevVelocities.resize(id+1);
		firsts.resize(id+1,true);
	}

	ParticleParameters * p = YADE_CAST<ParticleParameters*>(b.get());

	Real dt = Omega::instance().getTimeStep();

	if (!firsts[id])
		p->velocity = prevVelocities[id]+((Real)0.5)*dt*p->acceleration;

	prevVelocities[id] = p->velocity+((Real)0.5)*dt*p->acceleration;
	p->se3.position += prevVelocities[id]*dt;

	//cerr<<"#"<<body->getId()<<"dx="<<prevVelocities[id]*dt<<endl;

	firsts[id] = false;
}


YADE_PLUGIN();
