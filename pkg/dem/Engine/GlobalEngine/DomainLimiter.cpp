#include<yade/pkg-dem/DomainLimiter.hpp>

YADE_PLUGIN((DomainLimiter));

void DomainLimiter::action(){
	std::list<Body::id_t> out;
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if(!b) continue;
		const Vector3r& p(b->state->pos);
		if(p[0]<lo[0] || p[0]>hi[0] || p[1]<lo[1] || p[1]>hi[1] || p[2]<lo[2] || p[2]>hi[2]) out.push_back(b->id);
	}
	FOREACH(Body::id_t id, out){
		scene->bodies->erase(id);
		nDeleted++;
	}
}

