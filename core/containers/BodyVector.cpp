// © 2009 Václav Šmilauer <eudoxos@arcig.cz>
#include<yade/core/BodyVector.hpp>

#ifdef YADE_BOOST_SERIALIZATION
	BOOST_CLASS_EXPORT(BodyVector);
#endif


unsigned int BodyVector::findFreeId(){
	unsigned int max=bodies.size();
	for(; lowestFree<max; lowestFree++){
		if(!(bool)bodies[lowestFree]) return lowestFree;
	}
	return bodies.size();
}

unsigned int BodyVector::insert(shared_ptr<Body>& b){
	body_id_t newId=findFreeId();
	return insert(b,newId);
}

unsigned int BodyVector::insert(shared_ptr<Body>& b, unsigned int id){
	assert(id>=0);
	if((size_t)id>=bodies.size()) bodies.resize(id+1);
	BodyContainer::setId(b,id);
	bodies[id]=b;
	return id;
}
