// 2010 © Václav Šmilauer <eudoxos@arcig.cz>

#include "Scene.hpp"
#include "Body.hpp"
#include "BodyContainer.hpp"
#ifdef YADE_OPENMP
	#include<omp.h>
#endif


CREATE_LOGGER(BodyContainer);

BodyContainer::BodyContainer(){}

BodyContainer::~BodyContainer(){}

void BodyContainer::clear(){
	body.clear();
}

Body::id_t BodyContainer::insert(shared_ptr<Body>& b){
	Body::id_t newId=body.size();
	return insert(b,newId);
}

Body::id_t BodyContainer::insert(shared_ptr<Body>& b, Body::id_t id){
	assert(id>=0);
	if((size_t)id>=body.size()) body.resize(id+1);
	
	const shared_ptr<Scene>& scene=Omega::instance().getScene(); 
	b->iterBorn=scene->iter;
	b->timeBorn=scene->time;
	b->id=id;
	body[id]=b;
	return id;
}

bool BodyContainer::erase(Body::id_t id){
	if(!exists(id)) return false;
	
	const shared_ptr<Scene>& scene=Omega::instance().getScene();
	FOREACH(const shared_ptr<Interaction>& i, *scene->interactions){
		if((i->getId1()==id or i->getId2()==id)) {
			scene->interactions->requestErase(i->getId1(),i->getId2());
		}
	}
	body[id]=shared_ptr<Body>();
	
	return true;
}

