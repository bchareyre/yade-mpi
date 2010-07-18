// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include"StepDisplacer.hpp"
#include<yade/core/State.hpp>
#include<yade/core/Scene.hpp>
CREATE_LOGGER(StepDisplacer);
YADE_PLUGIN((StepDisplacer));

void StepDisplacer::action(){
	if(!isnan(deltaSe3.position[0])){
		LOG_WARN("Using StepDisplacer::deltaSe3 is deprecated, use StepDisplacer.mov and StepDisplacer.rot instead (setting automatically now).");
		mov=deltaSe3.position;
		rot=deltaSe3.orientation;
		deltaSe3.position=Vector3r(NaN,NaN,NaN); // to detect next manual change of deltaSe3
	}
	FOREACH(body_id_t id, subscribedBodies){
		const shared_ptr<Body>& b=Body::byId(id,scene);
		if(setVelocities){
			const Real& dt=scene->dt;
			b->state->vel=mov/dt;
			AngleAxisr aa(rot); aa.axis().normalize();
			b->state->angVel=aa.axis()*aa.angle()/dt;
			LOG_DEBUG("Angular velocity set to "<<aa.axis()*aa.angle()/dt<<". Axis="<<aa.axis()<<", angle="<<aa.angle());
		}
		if(!setVelocities || (setVelocities && !b->isDynamic())){
			b->state->pos+=mov;
			b->state->ori=rot*b->state->ori;
		}
	}
}

