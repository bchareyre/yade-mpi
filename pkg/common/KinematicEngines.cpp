
#include<yade/core/Scene.hpp>
#include<yade/pkg/common/KinematicEngines.hpp>
#include<yade/pkg/dem/Shop.hpp>
#include<yade/lib/smoothing/LinearInterpolate.hpp>

YADE_PLUGIN((KinematicEngine)(CombinedKinematicEngine)(TranslationEngine)(HarmonicMotionEngine)(RotationEngine)(HelixEngine)(InterpolatingHelixEngine)(HarmonicRotationEngine));

CREATE_LOGGER(KinematicEngine);

void KinematicEngine::action(){
	FOREACH(Body::id_t id,ids){
		assert(id<(Body::id_t)scene->bodies->size());
		Body* b=Body::byId(id,scene).get();
		if(b) b->state->vel=b->state->angVel=Vector3r::Zero();
	}
	apply(ids);
}

void CombinedKinematicEngine::action(){
	// reset first
	FOREACH(Body::id_t id,ids){
		assert(id<(Body::id_t)scene->bodies->size());
		Body* b=Body::byId(id,scene).get();
		if(b) b->state->vel=b->state->angVel=Vector3r::Zero();
	}
	// apply one engine after another
	FOREACH(const shared_ptr<KinematicEngine>& e, comb){
		e->scene=scene; e->apply(ids);
	}
}

const shared_ptr<CombinedKinematicEngine> CombinedKinematicEngine::fromTwo(const shared_ptr<KinematicEngine>& first, const shared_ptr<KinematicEngine>& second){
	shared_ptr<CombinedKinematicEngine> ret(new CombinedKinematicEngine);
	ret->ids=first->ids; ret->comb.push_back(first); ret->comb.push_back(second);
	return ret;
}


void TranslationEngine::apply(const vector<Body::id_t>& ids){
	#ifdef YADE_OPENMP
	const long size=ids.size();
	#pragma omp parallel for schedule(static)
	for(long i=0; i<size; i++){
		const Body::id_t& id=ids[i];
	#else
	FOREACH(Body::id_t id,ids){
	#endif
		assert(id<(Body::id_t)scene->bodies->size());
		Body* b=Body::byId(id,scene).get();
		if(!b) continue;
		b->state->vel+=velocity*translationAxis;
	}
}

void HarmonicMotionEngine::apply(const vector<Body::id_t>& ids){
	Vector3r w = f*2.0*Mathr::PI; 										 								//Angular frequency
	Vector3r velocity = ((((w*scene->time + fi).cwise().sin())*(-1.0)).cwise()*A).cwise()*w;	//Linear velocity at current time
	FOREACH(Body::id_t id,ids){
		assert(id<(Body::id_t)scene->bodies->size());
		Body* b=Body::byId(id,scene).get();
		if(!b) continue;
		b->state->vel+=velocity;
	}
}


void InterpolatingHelixEngine::apply(const vector<Body::id_t>& ids){
	Real virtTime=wrap ? Shop::periodicWrap(scene->time,*times.begin(),*times.rbegin()) : scene->time;
	angularVelocity=linearInterpolate<Real,Real>(virtTime,times,angularVelocities,_pos);
	linearVelocity=angularVelocity*slope;
	HelixEngine::apply(ids);
}

void HelixEngine::apply(const vector<Body::id_t>& ids){
	const Real& dt=scene->dt;
	angleTurned+=angularVelocity*dt;
	shared_ptr<BodyContainer> bodies = scene->bodies;
	FOREACH(Body::id_t id,ids){
		assert(id<(Body::id_t)bodies->size());
		Body* b=Body::byId(id,scene).get();
		if(!b) continue;
		b->state->vel+=linearVelocity*rotationAxis;
	}
	rotateAroundZero=true;
	RotationEngine::apply(ids);
}


void RotationEngine::apply(const vector<Body::id_t>& ids){
	Quaternionr q(AngleAxisr(angularVelocity*scene->dt,rotationAxis));
	#ifdef YADE_OPENMP
	const long size=ids.size();
	#pragma omp parallel for schedule(static)
	for(long i=0; i<size; i++){
		const Body::id_t& id=ids[i];
	#else
	FOREACH(Body::id_t id,ids){
	#endif
		assert(id<(Body::id_t)scene->bodies->size());
		Body* b=Body::byId(id,scene).get();
		if(!b) continue;
		b->state->angVel+=rotationAxis*angularVelocity;
		if(rotateAroundZero){
			const Vector3r l=b->state->pos-zeroPoint;
			Quaternionr q(AngleAxisr(angularVelocity*scene->dt,rotationAxis));
			Vector3r newPos=q*l+zeroPoint;
			b->state->vel+=Vector3r(newPos-b->state->pos)/scene->dt;
		}
	}
}

void HarmonicRotationEngine::apply(const vector<Body::id_t>& ids){
	const Real& time=scene->time;
	Real w = f*2.0*Mathr::PI; 			//Angular frequency
	angularVelocity = -1.0*A*w*sin(w*time + fi);
	RotationEngine::apply(ids);
}

