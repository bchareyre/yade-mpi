// 2010 © Václav Šmilauer <eudoxos@arcig.cz>

#include<yade/core/BodyContainer.hpp>
#include<yade/core/Body.hpp>

BOOST_CLASS_EXPORT(BodyContainer);

unsigned int BodyContainer::findFreeId(){
	unsigned int max=body.size();
	for(; lowestFree<max; lowestFree++){
		if(!(bool)body[lowestFree]) return lowestFree;
	}
	return body.size();
}

unsigned int BodyContainer::insert(shared_ptr<Body>& b){
	body_id_t newId=findFreeId();
	return insert(b,newId);
}

unsigned int BodyContainer::insert(shared_ptr<Body>& b, unsigned int id){
	assert(id>=0);
	if((size_t)id>=body.size()) body.resize(id+1);
	b->id=id;
	body[id]=b;
	return id;
}

BodyContainer::~BodyContainer(){}
