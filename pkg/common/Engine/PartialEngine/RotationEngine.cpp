// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2004 Janek Kozicki <cosurgi@berlios.de>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>

#include"RotationEngine.hpp"
#include<yade/core/Scene.hpp>
#include<yade/lib-base/Math.hpp>
#include<yade/pkg-dem/Shop.hpp>

#include<yade/pkg-common/LinearInterpolate.hpp>

YADE_PLUGIN((RotationEngine)(SpiralEngine)(InterpolatingSpiralEngine));

void InterpolatingSpiralEngine::action(){
	Real virtTime=wrap ? Shop::periodicWrap(scene->time,*times.begin(),*times.rbegin()) : scene->time;
	angularVelocity=linearInterpolate<Real,Real>(virtTime,times,angularVelocities,_pos);
	linearVelocity=angularVelocity*slope;
	SpiralEngine::action();
}

void SpiralEngine::action(){
	const Real& dt=scene->dt;
	axis.normalize();
	Quaternionr q(AngleAxisr(angularVelocity*dt,axis));
	angleTurned+=angularVelocity*dt;
	shared_ptr<BodyContainer> bodies = scene->bodies;
	FOREACH(Body::id_t id,ids){
		assert(id<(Body::id_t)bodies->size());
		Body* b=Body::byId(id,scene).get();
		if(!b) continue;
		// translation
		b->state->pos+=dt*linearVelocity*axis;
		// rotation
		b->state->pos=q*(b->state->pos-axisPt)+axisPt;
		b->state->ori=q*b->state->ori;
		b->state->ori.normalize(); // to make sure
		// bug: https://bugs.launchpad.net/yade/+bug/398089; since subscribed bodies are not dynamic (assumption), we have to set theri velocities here as well;
		// otherwise, their displacement will be missed in NewtonIntegrator and when using velocityBins, they will have no influence;
		// that can cause interactions to be missed, for example
		b->state->vel=linearVelocity*axis+angularVelocity*axis.cross(b->state->pos-axisPt); // check this...
		b->state->angVel=angularVelocity*axis;
	}
}


void RotationEngine::action(){
	rotationAxis.normalize();
	Quaternionr q(AngleAxisr(angularVelocity*scene->dt,rotationAxis));
	#ifdef YADE_OPENMP
	const long size=ids.size();
	#pragma omp parallel for schedule(static)
	for(long i=0; i<size; i++){
		const Body::id_t& id=ids[i];
	#else
	FOREACH(Body::id_t id,ids){
	#endif
		State* state=Body::byId(id,scene)->state.get();
		state->angVel=rotationAxis*angularVelocity;
		if(rotateAroundZero){
			const Vector3r l=state->pos-zeroPoint;
			state->pos=q*l+zeroPoint; 
			state->vel=state->angVel.cross(l);
		}
	state->ori=q*state->ori;
	state->ori.normalize();
	}
}

