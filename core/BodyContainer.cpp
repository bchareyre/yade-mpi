// 2010 © Václav Šmilauer <eudoxos@arcig.cz>

#include<yade/core/BodyContainer.hpp>
#include<yade/core/Body.hpp>
#ifdef YADE_OPENMP
	#include<omp.h>
#endif


CREATE_LOGGER(BodyContainer);
 
BodyContainer::BodyContainer(): lowestFree(0), 
	#ifdef YADE_OPENMP
		maxSubdomains(omp_get_max_threads()),
	#else
		maxSubdomains(1),
	#endif
	subDomainsLowestFree(maxSubdomains,0)
{}


BodyContainer::~BodyContainer(){}

void BodyContainer::clear(){
	body.clear(); lowestFree=0;
	subDomains.clear();
}

Body::id_t BodyContainer::findFreeId(){
	Body::id_t max=body.size();
	for(; lowestFree<max; lowestFree++){
		if(!(bool)body[lowestFree]) return lowestFree;
	}
	return body.size();
}

Body::id_t BodyContainer::findFreeDomainLocalId(int subDom){
	#ifdef YADE_OPENMP
		assert(subDom<(int)subDomains.size());
		Body::id_t max=subDomains[subDom].size();
		// LOG_TRACE("subDom="<<subDom<<", max="<<max);
		Body::id_t& low(subDomainsLowestFree[subDom]);
		for(; low<max; low++){
			if(!(bool)subDomains[subDom][low]) return low;
		}
		return subDomains[subDom].size();
	#else
		assert(false); // this function should never be called in OpenMP-less build
	#endif
}

Body::id_t BodyContainer::insert(shared_ptr<Body>& b){
	Body::id_t newId=findFreeId();
	return insert(b,newId);
}

Body::id_t BodyContainer::insert(shared_ptr<Body>& b, Body::id_t id){
	assert(id>=0);
	if((size_t)id>=body.size()) body.resize(id+1);
	b->id=id;
	body[id]=b;
	setBodySubdomain(b,0); // add it to subdomain #0; only effective if subdomains are set up
	return id;
}

void BodyContainer::clearSubdomains(){ subDomains.clear(); }
void BodyContainer::setupSubdomains(){ subDomains.clear(); subDomains.resize(maxSubdomains); }
// put given body to 
bool BodyContainer::setBodySubdomain(const shared_ptr<Body>& b, int subDom){
	#ifdef YADE_OPENMP
		assert(b==body[b->id]); // consistency check
		// subdomains not used
		if(subDomains.empty()){ b->subDomId=Body::ID_NONE; return false; }
		// there are subdomains, but it was requested to add the body to one that does not exists
		// it is an error condition, since traversal of subdomains would siletly skip this body
		// if(subDom>=(int)subDomains.size()) throw std::invalid_argument(("BodyContainer::setBodySubdomain: adding #"+lexical_cast<string>(b->id)+" to non-existent sub-domain "+lexical_cast<string>(subDom)).c_str());
		assert(subDom<(int)subDomains.size());
		Body::id_t localId=findFreeDomainLocalId(subDom);
		if(localId>=(Body::id_t)subDomains[subDom].size()) subDomains[subDom].resize(localId+1);
		subDomains[subDom][localId]=b;
		b->subDomId=domNumLocalId2subDomId(subDom,localId);
		return true;
	#else
		// no subdomains should exist on OpenMP-less builds, and bodies should not have subDomId set either
		assert(subDomains.empty()); assert(b->subDomId==Body::ID_NONE); return false;
	#endif
}

bool BodyContainer::erase(Body::id_t id){
	if(!exists(id)) return false;
	lowestFree=min(lowestFree,id);
	#ifdef YADE_OPENMP
		const shared_ptr<Body>& b=body[id];
		if(b->subDomId!=Body::ID_NONE){
			int subDom, localId;
			boost::tie(subDom,localId)=subDomId2domNumLocalId(b->subDomId);
			if(subDom<(int)subDomains.size() && localId<(Body::id_t)subDomains[subDom].size() && subDomains[subDom][localId]==b){
				subDomainsLowestFree[subDom]=min(subDomainsLowestFree[subDom],localId);
			} else {
				LOG_FATAL("Body::subDomId inconsistency detected for body #"<<id<<" while erasing (cross thumbs)!");
				if(subDom>=(int)subDomains.size()){ LOG_FATAL("\tsubDomain="<<subDom<<" (max "<<subDomains.size()-1<<")"); }
				else if(localId>=(Body::id_t)subDomains[subDom].size()){ LOG_FATAL("\tsubDomain="<<subDom<<", localId="<<localId<<" (max "<<subDomains[subDom].size()-1<<")"); }
				else if(subDomains[subDom][localId]!=b) { LOG_FATAL("\tsubDomains="<<subDom<<", localId="<<localId<<"; erasing #"<<id<<", subDomain record points to #"<<subDomains[subDom][localId]->id<<"."); }
			}
		}
	#else
		assert(body[id]->subDomId==Body::ID_NONE); // subDomId should never be defined for OpenMP-less builds
	#endif
	body[id]=shared_ptr<Body>();
	return true;
}

