// (c) 2018 Bruno Chareyre <bruno.chareyre@grenoble-inp.fr>

#define YADE_MPI
#ifdef YADE_MPI

#include "Subdomain.hpp"
#include<core/Scene.hpp>
#include<core/BodyContainer.hpp>
#include<core/State.hpp>
#include<pkg/common/Sphere.hpp>

YADE_PLUGIN((Subdomain));
CREATE_LOGGER(Subdomain);


YADE_PLUGIN((Bo1_Subdomain_Aabb)/*(Bo1_Facet_Aabb)(Bo1_Box_Aabb)*/);

void Subdomain::setMinMax()
{
	Scene* scene(Omega::instance().getScene().get());	// get scene
// 	Vector3r min, max;
	Real inf=std::numeric_limits<Real>::infinity();
	boundsMin=Vector3r(inf,inf,inf); boundsMax=Vector3r(-inf,-inf,-inf);
	if (ids.size()>0 and Body::byId(ids[0],scene)->subdomain != scene->subdomain) LOG_WARN("setMinMax executed with deprecated data (body->subdomain != scene->subdomain)");
	FOREACH(const Body::id_t& id, ids){
		const shared_ptr<Body>& b = Body::byId(id,scene);
		if(!b or !b->bound) continue;
		boundsMax=boundsMax.cwiseMax(b->bound->max);
		boundsMin=boundsMin.cwiseMin(b->bound->min);
	}
}

// inspired by Integrator::slaves_set (Integrator.hpp)
void Subdomain::intrs_set(const boost::python::list& source){
	int len=boost::python::len(source);
	intersections.clear();
	for(int i=0; i<len; i++){
		boost::python::extract<std::vector<Body::id_t> > serialGroup(source[i]);
		if (serialGroup.check()){ intersections.push_back(serialGroup()); continue; }
		cerr<<"  ... failed"<<endl;
		PyErr_SetString(PyExc_TypeError,"intersections should be provided as a list of list of ids");
		boost::python::throw_error_already_set();
	}
}

boost::python::list Subdomain::intrs_get(){
	boost::python::list ret;
	FOREACH(vector<Body::id_t >& grp, intersections){
		ret.append(boost::python::list(grp));
	}
	return ret;
}

void Bo1_Subdomain_Aabb::go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body* b){
// 	LOG_WARN("Bo1_Subdomain_Aabb::go()")
	Subdomain* domain = static_cast<Subdomain*>(cm.get());
	if(!bv){ bv=shared_ptr<Bound>(new Aabb);}
	Aabb* aabb=static_cast<Aabb*>(bv.get());
	
	if(!scene->isPeriodic){
		aabb->min=domain->boundsMin; aabb->max=domain->boundsMax;
		return;
	}
	LOG_WARN("Bo1_Subdomain_Aabb::go not implemented for periodic boundaries");
	// adjust box size along axes so that sphere doesn't stick out of the box even if sheared (i.e. parallelepiped)
// 	if(scene->cell->hasShear()) {
// // 		Vector3r halfSize = (aabbEnlargeFactor>0?aabbEnlargeFactor:1.)*Vector3r(sphere->radius,sphere->radius,sphere->radius);
// 		Vector3r refHalfSize(halfSize);
// 		const Vector3r& cos=scene->cell->getCos();
// 		for(int i=0; i<3; i++){
// 			//cerr<<"cos["<<i<<"]"<<cos[i]<<" ";
// 			int i1=(i+1)%3,i2=(i+2)%3;
// 			halfSize[i1]+=.5*refHalfSize[i1]*(1/cos[i]-1);
// 			halfSize[i2]+=.5*refHalfSize[i2]*(1/cos[i]-1);
// 		}
// 	}
// 	//cerr<<" || "<<halfSize<<endl;
// 	aabb->min = scene->cell->unshearPt(se3.position)-halfSize;
// 	aabb->max = scene->cell->unshearPt(se3.position)+halfSize;	
}






#endif




