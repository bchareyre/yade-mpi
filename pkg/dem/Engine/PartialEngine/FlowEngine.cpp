/*************************************************************************
*  Copyright (C) 2009 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "FlowEngine.hpp"
#include <yade/pkg-common/ParticleParameters.hpp>
#include<yade/core/Scene.hpp>
#include<yade/core/Scene.hpp>
#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>


FlowEngine::FlowEngine() : gravity(Vector3r::ZERO), isActivated(false)
{
dummyParameter = false;
}


FlowEngine::~FlowEngine()
{
}


void FlowEngine::applyCondition(Scene* ncb)
{
	if (!isActivated) return;
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	FOREACH(const shared_ptr<Body>& b, *bodies){
		// to avoid warning:
		b->physicalParameters; 

		// clump members are non-dynamic; they skip the rest of loop once their forces are properly taken into account, however
		//if (!b->isDynamic && !b->isClumpMember()) continue;
		
		///Access data (examples)
		#if 0
			RigidBodyParameters* rb = YADE_CAST<RigidBodyParameters*>(b->physicalParameters.get());
			const body_id_t& id=b->getId();
			Real vx = rb->velocity[0];
			Real rx = rb->angularVelocity[0];
			Real x = rb->se3.position[0];
		#endif
	}
	///Compute flow and and forces here

	
	
	
	
	
	///End Compute flow and and forces
	
	
	
	
	int Nspheres=100;
	for (long i=1; i<Nspheres; ++i)
	{

		//file >> id >> fx >> fy >> fz >> mx >> my >> mz;

		//Vector3r f (fx,fy,fz);
		//Vector3r t (mx,my,mz);
                
		//b->bex.addForce(id,f);
		//ncb->bex.addTorque(id,t);

	}
}

YADE_PLUGIN((FlowEngine));

YADE_REQUIRE_FEATURE(PHYSPAR);

