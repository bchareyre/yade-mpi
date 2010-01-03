// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2004 Janek Kozicki <cosurgi@berlios.de>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>

#include"RotationEngine.hpp"
#include<yade/core/Scene.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>
#include<yade/pkg-dem/Shop.hpp>

#include<yade/pkg-common/LinearInterpolate.hpp>

YADE_PLUGIN((RotationEngine)(SpiralEngine)(InterpolatingSpiralEngine));

void InterpolatingSpiralEngine::applyCondition(Scene* rb){
	Real virtTime=wrap ? Shop::periodicWrap(rb->simulationTime,*times.begin(),*times.rbegin()) : rb->simulationTime;
	angularVelocity=linearInterpolate<Real>(virtTime,times,angularVelocities,_pos);
	linearVelocity=angularVelocity*slope;
	SpiralEngine::applyCondition(rb);
}

void SpiralEngine::applyCondition(Scene* rb){
	Real dt=Omega::instance().getTimeStep();
	axis.Normalize();
	Quaternionr q;
	q.FromAxisAngle(axis,angularVelocity*dt);
	angleTurned+=angularVelocity*dt;
	shared_ptr<BodyContainer> bodies = rb->bodies;
	FOREACH(body_id_t id,subscribedBodies){
		assert(id<(body_id_t)bodies->size());
		Body* b=Body::byId(id,rb).get();
		if(!b) continue;
		// translation
		b->state->pos+=dt*linearVelocity*axis;
		// rotation
		b->state->pos=q*(b->state->pos-axisPt)+axisPt;
		b->state->ori=q*b->state->ori;
		b->state->ori.Normalize(); // to make sure
		// bug: https://bugs.launchpad.net/yade/+bug/398089; since subscribed bodies are not dynamic (assumption), we have to set theri velocities here as well;
		// otherwise, their displacement will be missed in NewtonIntegrator and when using velocityBins, they will have no influence;
		// that can cause interactions to be missed, for example
		b->state->vel=linearVelocity*axis+angularVelocity*axis.Cross(b->state->pos-axisPt); // check this...
		b->state->angVel=angularVelocity*axis;
	}
}

RotationEngine::RotationEngine(){
	rotateAroundZero = false;
	zeroPoint = Vector3r(0,0,0);
}



void RotationEngine::applyCondition(Scene*){
   rotationAxis.Normalize();
	Quaternionr q;
	q.FromAxisAngle(rotationAxis,angularVelocity*scene->dt);
	FOREACH(body_id_t id,subscribedBodies){
		State* rb=Body::byId(id,scene)->state.get();
		rb->angVel=rotationAxis*angularVelocity;
		if(rotateAroundZero){
			const Vector3r l=rb->pos-zeroPoint;
			rb->pos=q*l+zeroPoint; 
			rb->vel=rb->angVel.Cross(l);
		}
		rb->ori=q*rb->ori;
		rb->ori.Normalize();
	}
}

