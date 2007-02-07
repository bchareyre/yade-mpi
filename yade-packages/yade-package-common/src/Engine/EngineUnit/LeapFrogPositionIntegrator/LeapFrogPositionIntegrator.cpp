/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "LeapFrogPositionIntegrator.hpp" 
#include "ParticleParameters.hpp"
#include <yade/yade-core/Omega.hpp>

// FIXME : should we pass timestep as parameter of functor
// FIXME : what's with timestepper
void LeapFrogPositionIntegrator::go(       const shared_ptr<PhysicalParameters>& b
						, Body* body)
{
	#ifdef HIGHLEVEL_CLUMPS
		if(!body->isDynamic || body->clumpId>=0) return; // if clump member, integration will be done in clump's integrator and position of this body modified directly
	#else
	if(!body->isDynamic) return;
	#endif

	unsigned int id = body->getId();
	
	if (prevVelocities.size()<=id)
	{
		prevVelocities.resize(id+1);
		firsts.resize(id+1,true);
	}

	ParticleParameters * p = dynamic_cast<ParticleParameters*>(b.get());

	Real dt = Omega::instance().getTimeStep();

	if (!firsts[id])
		p->velocity = prevVelocities[id]+((Real)0.5)*dt*p->acceleration;

	prevVelocities[id] = p->velocity+((Real)0.5)*dt*p->acceleration;
	p->se3.position += prevVelocities[id]*dt;

	firsts[id] = false;
}


