// (c) 2018 Bruno Chareyre <bruno.chareyre@grenoble-inp.fr>
 
#pragma once

#include<core/Body.hpp>
#include<lib/base/Logging.hpp>
#include<lib/base/Math.hpp>
// #include<core/PartialEngine.hpp>
#include <pkg/common/Aabb.hpp>
#include <pkg/common/Dispatching.hpp>
#include <lib/pyutil/numpy_boost.hpp>

class NewtonIntegrator;


class Subdomain: public Shape {
	public:
// 	typedef std::map<Body::id_t,std::vector<Body::id_t> > IntersectionMap; // the lists of bodies from other subdomaines intersecting this one
	//Map fails...
	typedef std::vector< std::vector<Body::id_t> > IntersectionMap; // the lists of bodies from other subdomaines intersecting this one
// 	typedef numpy_boost<Body::id_t,2> testType({1,1}) ;
	boost::python::list intrs_get();
	void intrs_set(const boost::python::list& intrs);
//     boost::python::dict members_get();

	//WARNING: precondition: the members bounds have been dispatched already, else we re-use old values. Carefull if subdomain is not at the end of O.bodies
	void setMinMax();
		
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Subdomain,Shape,"The bounding box of a mpi subdomain",
// 		((testType, testArray,testType({0,0}),,""))
		((Body::id_t,subDomainIndex,-1,,"the index of this sudomain in the decomposition (it differs from the body's id in general)"))
		((Real,extraLength,0,,"verlet dist for the subdomain, added to bodies verletDist"))
		((Vector3r,boundsMin,Vector3r(NaN,NaN,NaN),,"min corner of all bboxes of members; differs from effective domain bounds by the extra length (sweepLength)"))
		((Vector3r,boundsMax,Vector3r(NaN,NaN,NaN),,"max corner of all bboxes of members; differs from effective domain bounds by the extra length (sweepLength)"))
		((IntersectionMap,intersections,IntersectionMap(),Attr::hidden,"[will be overridden below]"))
		((vector<Body::id_t>,ids,vector<Body::id_t>(),,"Ids of owned particles.")) //FIXME
		,/*ctor*/ createIndex();
		,/*py*/ /*.add_property("members",&Clump::members_get,"Return clump members as {'id1':(relPos,relOri),...}")*/
		.def("setMinMax",&Subdomain::setMinMax,"returns bounding min-max based on members bounds. precondition: the members bounds have been dispatched already, else we re-use old values. Carefull if subdomain is not at the end of O.bodies.")
		.add_property("intersections",&Subdomain::intrs_get,&Subdomain::intrs_set,"lists of bodies from other subdomains intersecting this one. WARNING: only assignement and concatenation allowed")
	);
	DECLARE_LOGGER;
	REGISTER_CLASS_INDEX(Subdomain,Shape);
};
REGISTER_SERIALIZABLE(Subdomain);

class Bo1_Subdomain_Aabb : public BoundFunctor{
	public:
		void go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body*);
	FUNCTOR1D(Subdomain);
	YADE_CLASS_BASE_DOC(Bo1_Subdomain_Aabb,BoundFunctor,"Creates/updates an :yref:`Aabb` of a :yref:`Facet`.");
};
REGISTER_SERIALIZABLE(Bo1_Subdomain_Aabb);


