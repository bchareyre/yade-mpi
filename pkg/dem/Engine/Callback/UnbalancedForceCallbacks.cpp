#include<yade/pkg-dem/UnbalancedForceCallbacks.hpp>
#include<yade/pkg-common/NormShearPhys.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/Scene.hpp>

YADE_PLUGIN((SumIntrForcesCb)(SumBodyForcesCb));

int SumIntrForcesCb::int0=0;
Real SumIntrForcesCb::Real0=0;

IntrCallback::FuncPtr SumIntrForcesCb::stepInit(){
	// if(scene->currentIteration%100 != 0) return NULL;

	cerr<<"("<<(Real)force<<","<<(int)numIntr<<")";
	// reset accumulators
	force.reset(); numIntr.reset();
	// return function pointer
	return &SumIntrForcesCb::go;
}

void SumIntrForcesCb::go(IntrCallback* _self, Interaction* i){
	SumIntrForcesCb* self=static_cast<SumIntrForcesCb*>(_self);
	NormShearPhys* nsp=YADE_CAST<NormShearPhys*>(i->interactionPhysics.get());
	assert(nsp!=NULL); // only effective in debug mode
	Vector3r f=nsp->normalForce+nsp->shearForce;
	if(f==Vector3r::ZERO) return;
	self->numIntr+=1;
	self->force+=f.Length();
	//cerr<<"[cb#"<<i->getId1()<<"+"<<i->getId2()<<"]";
}

BodyCallback::FuncPtr SumBodyForcesCb::stepInit(){
	cerr<<"{"<<(Real)force<<","<<(int)numBodies<<"}";
	force.reset(); numBodies.reset(); // reset accumulators
	return &SumBodyForcesCb::go;
}
void SumBodyForcesCb::go(BodyCallback* _self, Body* b){
	if(!b->isDynamic) return;
	cerr<<"["<<omp_get_thread_num()<<",#"<<b->id<<"]";
	SumBodyForcesCb* self=static_cast<SumBodyForcesCb*>(_self);
	self->numBodies+=1;
	//self->scene->forces.sync();
	self->force+=self->scene->forces.getForce(b->id).Length();
}
