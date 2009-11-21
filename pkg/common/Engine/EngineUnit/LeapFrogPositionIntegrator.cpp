/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "LeapFrogPositionIntegrator.hpp" 
#include<yade/pkg-common/ParticleParameters.hpp>
#include<yade/core/Omega.hpp>


void LeapFrogPositionIntegrator::go(const shared_ptr<PhysicalParameters>& b
						, Body* body, BexContainer& bex)
{
	if(!body->isDynamic) return;

// 	unsigned int id = body->getId();
	
// 	if (prevVelocities.size()<=id)
// 	{
// 		prevVelocities.resize(id+1);
// 		firsts.resize(id+1,true);
// 	}

	ParticleParameters * p = YADE_CAST<ParticleParameters*>(b.get());

	Real dt = Omega::instance().getTimeStep();

// 	if (!firsts[id])
// 		p->velocity = prevVelocities[id]+((Real)0.5)*dt*p->acceleration;

	if(p->blockedDOFs==0) p->velocity = p->velocity+dt*p->acceleration;
	else{
		if((p->blockedDOFs & PhysicalParameters::DOF_X)==0) p->velocity[0]+=dt*p->acceleration[0];
		if((p->blockedDOFs & PhysicalParameters::DOF_Y)==0) p->velocity[1]+=dt*p->acceleration[1];
		if((p->blockedDOFs & PhysicalParameters::DOF_Z)==0) p->velocity[2]+=dt*p->acceleration[2];
	}
	p->se3.position += p->velocity*dt + bex.getMove(body->getId());

	//cerr<<"#"<<body->getId()<<"dx="<<prevVelocities[id]*dt<<endl;

	//firsts[id] = false;
}


YADE_PLUGIN((LeapFrogPositionIntegrator));

YADE_REQUIRE_FEATURE(PHYSPAR);

