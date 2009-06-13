// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#include<boost/python.hpp>
#include<yade/extra/boost_python_len.hpp>
#include<yade/lib-base/Logging.hpp>
#include<yade/lib-base/yadeWm3.hpp>
#include<Wm3Vector3.h>

using namespace boost;
using namespace std;
#ifdef LOG4CXX
	log4cxx::LoggerPtr logger=log4cxx::Logger::getLogger("yade.predicates");
#endif

/*
This file contains various predicates that say whether a given point is within the solid,
or, not closer than "pad" to its boundary, if pad is nonzero
Besides the (point,pad) operator, each predicate defines aabb() method that returns
(min,max) tuple defining minimum and maximum point of axis-aligned bounding box 
for the predicate.

These classes are primarily used for yade.pack.* functions creating packings.
See scripts/test/regular-sphere-pack.py for an example.

*/

// aux functions
python::tuple vec2tuple(const Vector3r& v){return boost::python::make_tuple(v[0],v[1],v[2]);}
Vector3r tuple2vec(const python::tuple& t){return Vector3r(python::extract<double>(t[0])(),python::extract<double>(t[1])(),python::extract<double>(t[2])());}
void ttuple2vvec(const python::tuple& t, Vector3r& v1, Vector3r& v2){ v1=tuple2vec(python::extract<python::tuple>(t[0])()); v2=tuple2vec(python::extract<python::tuple>(t[1])()); }
python::tuple vvec2ttuple(const Vector3r&v1, const Vector3r&v2){ return python::make_tuple(vec2tuple(v1),vec2tuple(v2)); }


/*! Sphere predicate */
class inSphere {
	Vector3r center; Real radius;
public:
	inSphere(python::tuple _center, Real _radius){center=tuple2vec(_center); radius=_radius;}
	bool operator()(python::tuple _pt, Real pad=0.){
		Vector3r pt=tuple2vec(_pt);
		return ((pt-center).Length()-pad<=radius-pad);
	}
	python::tuple aabb(){return vvec2ttuple(Vector3r(center[0]-radius,center[1]-radius,center[2]-radius),Vector3r(center[0]+radius,center[1]+radius,center[2]+radius));}
};

/* Axis-aligned box predicate */
class inAlignedBox{
	Vector3r mn, mx;
public:
	inAlignedBox(python::tuple _mn, python::tuple _mx){mn=tuple2vec(_mn); mx=tuple2vec(_mx);}
	bool operator()(python::tuple _pt, Real pad=0.){
		Vector3r pt=tuple2vec(_pt);
		return
			mn[0]+pad<=pt[0] && mx[0]-pad>=pt[0] &&
			mn[1]+pad<=pt[1] && mx[1]-pad>=pt[1] &&
			mn[2]+pad<=pt[2] && mx[2]-pad>=pt[2];
	}
	python::tuple aabb(){ return vvec2ttuple(mn,mx); }
};

/* Arbitrarily oriented cylinder predicate */
class inCylinder{
	Vector3r c1,c2,c12; Real radius,ht;
public:
	inCylinder(python::tuple _c1, python::tuple _c2, Real _radius){c1=tuple2vec(_c1); c2=tuple2vec(_c2); c12=c2-c1; radius=_radius; ht=c12.Length(); }
	bool operator()(python::tuple _pt, Real pad=0.){
		Vector3r pt=tuple2vec(_pt);
		Real u=(pt.Dot(c12)-c1.Dot(c12))/(ht*ht); // normalized coordinate along the c1--c2 axis
		if((u*ht<0+pad) || (u*ht>ht-pad)) return false; // out of cylinder along the axis
		Real axisDist=((pt-c1).Cross(pt-c2)).Length()/ht;
		if(axisDist>radius-pad) return false;
		return true;
	}
	python::tuple aabb(){
		// see http://www.gamedev.net/community/forums/topic.asp?topic_id=338522&forum_id=20&gforum_id=0 for the algorithm
		Vector3r& A(c1); Vector3r& B(c2); 
		Vector3r k(
			sqrt((pow(A[1]-B[1],2)+pow(A[2]-B[2],2)))/ht,
			sqrt((pow(A[0]-B[0],2)+pow(A[2]-B[2],2)))/ht,
			sqrt((pow(A[0]-B[0],2)+pow(A[1]-B[1],2)))/ht);
		Vector3r mn(min(A[0],B[0]),min(A[1],B[1]),min(A[2],B[2])), mx(max(A[0],B[0]),max(A[1],B[1]),max(A[2],B[2]));
		return vvec2ttuple(mn-radius*k,mx+radius*k);
	}
};

/* Oriented hyperboloid predicate (cylinder as special case).

See http://mathworld.wolfram.com/Hyperboloid.html for the parametrization and meaning of symbols
*/
class inHyperboloid{
	Vector3r c1,c2,c12; Real R,a,ht,c;
public:
	inHyperboloid(python::tuple _c1, python::tuple _c2, Real _R, Real _r){
		c1=tuple2vec(_c1); c2=tuple2vec(_c2); R=_R; a=_r;
		c12=c2-c1; ht=c12.Length();
		Real uMax=sqrt(pow(R/a,2)-1); c=ht/(2*uMax);
	}
	bool operator()(python::tuple _pt, Real pad=0.){
		Vector3r pt=tuple2vec(_pt);
		Real v=(pt.Dot(c12)-c1.Dot(c12))/(ht*ht); // normalized coordinate along the c1--c2 axis
		if((v*ht<0+pad) || (v*ht>ht-pad)) return false; // out of cylinder along the axis
		Real u=(v-.5)*ht/c; // u from the wolfram parametrization; u is 0 in the center
		Real rHere=a*sqrt(1+u*u); // pad is taken perpendicular to the axis, not to the surface (inaccurate)
		Real axisDist=((pt-c1).Cross(pt-c2)).Length()/ht;
		if(axisDist>rHere-pad) return false;
		return true;
	}
	python::tuple aabb(){
		// the lazy way
		return inCylinder(vec2tuple(c1),vec2tuple(c2),R).aabb();
	}
};

BOOST_PYTHON_MODULE(_packPredicates){
	boost::python::class_<inSphere>("inSphere","Sphere predicate.",python::init<python::tuple,Real>(python::args("center","radius"),"Ctor taking center (as a 3-tuple) and radius"))
		.def("__call__",&inSphere::operator(),"Tell whether given point lies within this sphere, still having 'pad' space to the solid boundary").def("aabb",&inSphere::aabb,"Return minimum and maximum values for AABB");
	boost::python::class_<inAlignedBox>("inAlignedBox","Axis-aligned box predicate",python::init<python::tuple,python::tuple>(python::args("minAABB","maxAABB"),"Ctor taking minumum and maximum points of the box (as 3-tuples)."))
		.def("__call__",&inAlignedBox::operator(),"Tell whether given point lies within this box, still having 'pad' space to the solid boundary").def("aabb",&inAlignedBox::aabb,"Return minimum and maximum values for AABB");
	boost::python::class_<inCylinder>("inCylinder","Cylinder predicate",python::init<python::tuple,python::tuple,Real>(python::args("centerBottom","centerTop","radius"),"Ctor taking centers of the lateral walls (as 3-tuples) and radius."))
		.def("__call__",&inCylinder::operator(),"Tell whether given point lies within this cylinder, still having 'pad' space to the solid boundary").def("aabb",&inCylinder::aabb,"Return minimum and maximum values for AABB");
	boost::python::class_<inHyperboloid>("inHyperboloid","Hyperboloid predicate",python::init<python::tuple,python::tuple,Real,Real>(python::args("centerBottom","centerTop","radius","skirt"),"Ctor taking centers of the lateral walls (as 3-tuples), radius at bases and skirt (middle radius)."))
		.def("__call__",&inHyperboloid::operator(),"Tell whether given point lies within this hyperboloid, still having 'pad' space to the solid boundary\n(not accurate, since distance perpendicular to the axis, not the surface, is taken in account)").def("aabb",&inHyperboloid::aabb,"Return minimum and maximum values for AABB");
}

