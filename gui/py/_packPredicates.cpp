// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#include<boost/python.hpp>
#include<yade/extra/boost_python_len.hpp>
#include<yade/lib-base/Logging.hpp>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>
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

class Predicate{
	public:
		virtual bool operator() (python::tuple pt,Real pad=0.) const {throw logic_error("Calling virtual operator() of an abstract class Predicate.");}
		virtual python::tuple aabb() const {throw logic_error("Calling virtual aabb() of an abstract class Predicate.");}
};
// make the pad parameter optional
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PredicateCall_overloads,operator(),1,2);

/*********************************************************************************
****************** Boolean operations on predicates ******************************
*********************************************************************************/

class PredicateBoolean: public Predicate{
	protected:
		const shared_ptr<Predicate> A,B;
	public:
		PredicateBoolean(const shared_ptr<Predicate> _A, const shared_ptr<Predicate> _B): A(_A), B(_B){}
		const shared_ptr<Predicate> getA(){ return A;}
		const shared_ptr<Predicate> getB(){ return B;}
};

// http://www.linuxtopia.org/online_books/programming_books/python_programming/python_ch16s03.html
class PredicateUnion: public PredicateBoolean{
	public:
		PredicateUnion(const shared_ptr<Predicate> _A, const shared_ptr<Predicate> _B): PredicateBoolean(_A,_B){}
		virtual bool operator()(python::tuple pt,Real pad) const {return (*A)(pt,pad)||(*B)(pt,pad);}
		virtual python::tuple aabb() const { Vector3r minA,maxA,minB,maxB; ttuple2vvec(A->aabb(),minA,maxA); ttuple2vvec(B->aabb(),minB,maxB); return vvec2ttuple(componentMinVector(minA,minB),componentMaxVector(maxA,maxB));}
};
PredicateUnion makeUnion(const shared_ptr<Predicate>& A, const shared_ptr<Predicate>& B){ return PredicateUnion(A,B);}

class PredicateIntersection: public PredicateBoolean{
	public:
		PredicateIntersection(const shared_ptr<Predicate> _A, const shared_ptr<Predicate> _B): PredicateBoolean(_A,_B){}
		virtual bool operator()(python::tuple pt,Real pad) const {return (*A)(pt,pad) && (*B)(pt,pad);}
		virtual python::tuple aabb() const { Vector3r minA,maxA,minB,maxB; ttuple2vvec(A->aabb(),minA,maxA); ttuple2vvec(B->aabb(),minB,maxB); return vvec2ttuple(componentMaxVector(minA,minB),componentMinVector(maxA,maxB));}
};
PredicateIntersection makeIntersection(const shared_ptr<Predicate>& A, const shared_ptr<Predicate>& B){ return PredicateIntersection(A,B);}

class PredicateDifference: public PredicateBoolean{
	public:
		PredicateDifference(const shared_ptr<Predicate> _A, const shared_ptr<Predicate> _B): PredicateBoolean(_A,_B){}
		virtual bool operator()(python::tuple pt,Real pad) const {return (*A)(pt,pad) && !(*B)(pt,pad);}
		virtual python::tuple aabb() const { return A->aabb(); }
};
PredicateDifference makeDifference(const shared_ptr<Predicate>& A, const shared_ptr<Predicate>& B){ return PredicateDifference(A,B);}

class PredicateSymmetricDifference: public PredicateBoolean{
	public:
		PredicateSymmetricDifference(const shared_ptr<Predicate> _A, const shared_ptr<Predicate> _B): PredicateBoolean(_A,_B){}
		virtual bool operator()(python::tuple pt,Real pad) const {bool inA=(*A)(pt,pad), inB=(*B)(pt,pad); return (inA && !inB) || (!inA && inB);}
		virtual python::tuple aabb() const { Vector3r minA,maxA,minB,maxB; ttuple2vvec(A->aabb(),minA,maxA); ttuple2vvec(B->aabb(),minB,maxB); return vvec2ttuple(componentMinVector(minA,minB),componentMaxVector(maxA,maxB));}
};
PredicateSymmetricDifference makeSymmetricDifference(const shared_ptr<Predicate>& A, const shared_ptr<Predicate>& B){ return PredicateSymmetricDifference(A,B);}

/*********************************************************************************
****************************** Primitive predicates ******************************
*********************************************************************************/


/*! Sphere predicate */
class inSphere: public Predicate {
	Vector3r center; Real radius;
public:
	inSphere(python::tuple _center, Real _radius){center=tuple2vec(_center); radius=_radius;}
	virtual bool operator()(python::tuple _pt, Real pad=0.) const {
		Vector3r pt=tuple2vec(_pt);
		return ((pt-center).Length()-pad<=radius-pad);
	}
	virtual python::tuple aabb() const {return vvec2ttuple(Vector3r(center[0]-radius,center[1]-radius,center[2]-radius),Vector3r(center[0]+radius,center[1]+radius,center[2]+radius));}
};

/*! Axis-aligned box predicate */
class inAlignedBox: public Predicate{
	Vector3r mn, mx;
public:
	inAlignedBox(python::tuple _mn, python::tuple _mx){mn=tuple2vec(_mn); mx=tuple2vec(_mx);}
	virtual bool operator()(python::tuple _pt, Real pad=0.) const {
		Vector3r pt=tuple2vec(_pt);
		return
			mn[0]+pad<=pt[0] && mx[0]-pad>=pt[0] &&
			mn[1]+pad<=pt[1] && mx[1]-pad>=pt[1] &&
			mn[2]+pad<=pt[2] && mx[2]-pad>=pt[2];
	}
	virtual python::tuple aabb() const { return vvec2ttuple(mn,mx); }
};

/*! Arbitrarily oriented cylinder predicate */
class inCylinder: public Predicate{
	Vector3r c1,c2,c12; Real radius,ht;
public:
	inCylinder(python::tuple _c1, python::tuple _c2, Real _radius){c1=tuple2vec(_c1); c2=tuple2vec(_c2); c12=c2-c1; radius=_radius; ht=c12.Length(); }
	bool operator()(python::tuple _pt, Real pad=0.) const {
		Vector3r pt=tuple2vec(_pt);
		Real u=(pt.Dot(c12)-c1.Dot(c12))/(ht*ht); // normalized coordinate along the c1--c2 axis
		if((u*ht<0+pad) || (u*ht>ht-pad)) return false; // out of cylinder along the axis
		Real axisDist=((pt-c1).Cross(pt-c2)).Length()/ht;
		if(axisDist>radius-pad) return false;
		return true;
	}
	python::tuple aabb() const {
		// see http://www.gamedev.net/community/forums/topic.asp?topic_id=338522&forum_id=20&gforum_id=0 for the algorithm
		const Vector3r& A(c1); const Vector3r& B(c2); 
		Vector3r k(
			sqrt((pow(A[1]-B[1],2)+pow(A[2]-B[2],2)))/ht,
			sqrt((pow(A[0]-B[0],2)+pow(A[2]-B[2],2)))/ht,
			sqrt((pow(A[0]-B[0],2)+pow(A[1]-B[1],2)))/ht);
		Vector3r mn(min(A[0],B[0]),min(A[1],B[1]),min(A[2],B[2])), mx(max(A[0],B[0]),max(A[1],B[1]),max(A[2],B[2]));
		return vvec2ttuple(mn-radius*k,mx+radius*k);
	}
};

/*! Oriented hyperboloid predicate (cylinder as special case).

See http://mathworld.wolfram.com/Hyperboloid.html for the parametrization and meaning of symbols
*/
class inHyperboloid: public Predicate{
	Vector3r c1,c2,c12; Real R,a,ht,c;
public:
	inHyperboloid(python::tuple _c1, python::tuple _c2, Real _R, Real _r){
		c1=tuple2vec(_c1); c2=tuple2vec(_c2); R=_R; a=_r;
		c12=c2-c1; ht=c12.Length();
		Real uMax=sqrt(pow(R/a,2)-1); c=ht/(2*uMax);
	}
	// WARN: this is not accurate, since padding is taken as perpendicular to the axis, not the the surface
	bool operator()(python::tuple _pt, Real pad=0.) const {
		Vector3r pt=tuple2vec(_pt);
		Real v=(pt.Dot(c12)-c1.Dot(c12))/(ht*ht); // normalized coordinate along the c1--c2 axis
		if((v*ht<0+pad) || (v*ht>ht-pad)) return false; // out of cylinder along the axis
		Real u=(v-.5)*ht/c; // u from the wolfram parametrization; u is 0 in the center
		Real rHere=a*sqrt(1+u*u); // pad is taken perpendicular to the axis, not to the surface (inaccurate)
		Real axisDist=((pt-c1).Cross(pt-c2)).Length()/ht;
		if(axisDist>rHere-pad) return false;
		return true;
	}
	python::tuple aabb() const {
		// the lazy way
		return inCylinder(vec2tuple(c1),vec2tuple(c2),R).aabb();
	}
};

/*! Axis-aligned ellipsoid predicate */
class inEllipsoid: public Predicate{
	Vector3r c, abc;
public:
	inEllipsoid(python::tuple _c, python::tuple _abc) {c=tuple2vec(_c); abc=tuple2vec(_abc);}
	bool operator()(python::tuple _pt, Real pad=0.) const {
		Vector3r pt=tuple2vec(_pt);
		//Define the ellipsoid X-coordinate of given Y and Z
		Real x = sqrt((1-pow((pt[1]-c[1]),2)/((abc[1]-pad)*(abc[1]-pad))-pow((pt[2]-c[2]),2)/((abc[2]-pad)*(abc[2]-pad)))*((abc[0]-pad)*(abc[0]-pad)))+c[0]; 
		Vector3r edgeEllipsoid(x,pt[1],pt[2]); // create a vector of these 3 coordinates
		//check whether given coordinates lie inside ellipsoid or not
		if ((pt-c).Length()<=(edgeEllipsoid-c).Length()) return true;
		else return false;
	}
	python::tuple aabb() const {
		const Vector3r& center(c); const Vector3r& ABC(abc);
		return vvec2ttuple(Vector3r(center[0]-ABC[0],center[1]-ABC[1],center[2]-ABC[2]),Vector3r(center[0]+ABC[0],center[1]+ABC[1],center[2]+ABC[2]));
	}
};

/*! Negative notch predicate.

Use intersection (& operator) of another predicate with notInNotch to create notched solid.


		
		geometry explanation:
		
			c: the center
			normalHalfHt (in constructor): A-C
			inside: perpendicular to notch edge, points inside the notch (unit vector)
			normal: perpendicular to inside, perpendicular to both notch planes

		          ↑ distUp        A
		-------------------------
		                        | C
		         inside(unit) ← * → distInPlane
		                        |
		-------------------------
		          ↓ distDown      B

*/
class notInNotch: public Predicate{
	Vector3r c, inside, normal; Real halfHt;
public:
	notInNotch(python::tuple _c, python::tuple _inside, python::tuple _normalHalfHt){ Vector3r normalHalfHt=tuple2vec(_normalHalfHt); halfHt=normalHalfHt.Normalize(); normal=normalHalfHt; inside=tuple2vec(_inside); inside.Normalize(); c=tuple2vec(_c); }
	bool operator()(python::tuple _pt, Real pad=0.) const {
		Vector3r pt=tuple2vec(_pt);
		Real distUp=normal.Dot(pt-c)-halfHt, distDown=-normal.Dot(pt-c)-halfHt, distInPlane=-inside.Dot(pt-c);
		if(distInPlane>=pad) return true;
		if(distUp     >=pad) return true;
		if(distDown   >=pad) return true;
		if(distInPlane<0) return false;
		if(distUp  >0) return sqrt(pow(distInPlane,2)+pow(distUp,2))>=pad;
		if(distDown>0) return sqrt(pow(distInPlane,2)+pow(distUp,2))>=pad;
		// between both notch planes, closer to the edge than pad (distInPlane<pad)
		return false;
	}
	// aabb here doesn't make any sense since we are negated. Return just the center point.
	python::tuple aabb() const { return vvec2ttuple(c,c); }
};


BOOST_PYTHON_MODULE(_packPredicates){
	python::class_<Predicate, shared_ptr<Predicate> >("Predicate")
		.def("__call__",&Predicate::operator(),PredicateCall_overloads(python::args("point","padding"),"Tell whether given point lies within this sphere, still having 'pad' space to the solid boundary"))
		.def("aabb",&Predicate::aabb,"Return minimum and maximum values for AABB")
		.def("__or__",makeUnion).def("__and__",makeIntersection).def("__sub__",makeDifference).def("__xor__",makeSymmetricDifference);
	python::class_<PredicateBoolean,python::bases<Predicate> >("PredicateBoolean","Boolean operation on 2 predicates (abstract class)",python::no_init)
		.add_property("A",&PredicateBoolean::getA).add_property("B",&PredicateBoolean::getB);
	python::class_<PredicateUnion,python::bases<PredicateBoolean> >("PredicateUnion","Union of 2 predicates",python::init<shared_ptr<Predicate>,shared_ptr<Predicate> >());
	python::class_<PredicateIntersection,python::bases<PredicateBoolean> >("PredicateIntersection","Intersection of 2 predicates",python::init<shared_ptr<Predicate>,shared_ptr<Predicate> >());
	python::class_<PredicateDifference,python::bases<PredicateBoolean> >("PredicateDifference","Difference of 2 predicates",python::init<shared_ptr<Predicate>,shared_ptr<Predicate> >());
	python::class_<PredicateSymmetricDifference,python::bases<PredicateBoolean> >("PredicateSymmetricDifference","SymmetricDifference of 2 predicates",python::init<shared_ptr<Predicate>,shared_ptr<Predicate> >());
	python::class_<inSphere,python::bases<Predicate> >("inSphere","Sphere predicate.",python::init<python::tuple,Real>(python::args("center","radius"),"Ctor taking center (as a 3-tuple) and radius"));
	python::class_<inAlignedBox,python::bases<Predicate> >("inAlignedBox","Axis-aligned box predicate",python::init<python::tuple,python::tuple>(python::args("minAABB","maxAABB"),"Ctor taking minumum and maximum points of the box (as 3-tuples)."));
	python::class_<inCylinder,python::bases<Predicate> >("inCylinder","Cylinder predicate",python::init<python::tuple,python::tuple,Real>(python::args("centerBottom","centerTop","radius"),"Ctor taking centers of the lateral walls (as 3-tuples) and radius."));
	python::class_<inHyperboloid,python::bases<Predicate> >("inHyperboloid","Hyperboloid predicate",python::init<python::tuple,python::tuple,Real,Real>(python::args("centerBottom","centerTop","radius","skirt"),"Ctor taking centers of the lateral walls (as 3-tuples), radius at bases and skirt (middle radius)."));
	python::class_<inEllipsoid,python::bases<Predicate> >("inEllipsoid","Ellipsoid predicate",python::init<python::tuple,python::tuple>(python::args("centerPoint","abc"),"Ctor taking center of the ellipsoid (3-tuple) and its 3 radii (3-tuple)."));
	python::class_<notInNotch,python::bases<Predicate> >("notInNotch","Outside of infinite, rectangle-shaped notch predicate",python::init<python::tuple,python::tuple,python::tuple>(python::args("centerPoint","insideNotchVector","orientedHalfAperture"),"Ctor taking point in the symmetry plane, vector pointing inside the notch perpendicular to the edge, vector perpendicular to the edge and to the plane normal, with length equal to half of the aperture.")); 

}

