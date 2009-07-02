// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#include<boost/python.hpp>
#include<yade/extra/boost_python_len.hpp>
#include<yade/lib-base/Logging.hpp>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>
// #include<yade/gui-py/_utils.hpp> // will be: yade/lib-py/_utils.hpp> at some point
#include<Wm3Vector3.h>

#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>

#include<yade/extra/Shop.hpp>

using namespace boost;
using namespace std;
#ifdef LOG4CXX
	log4cxx::LoggerPtr logger=log4cxx::Logger::getLogger("yade.pack.spheres");
#endif

// copied from _packPredicates.cpp :-(
python::tuple vec2tuple(const Vector3r& v){return boost::python::make_tuple(v[0],v[1],v[2]);}
Vector3r tuple2vec(const python::tuple& t){return Vector3r(python::extract<double>(t[0])(),python::extract<double>(t[1])(),python::extract<double>(t[2])());}
Vector3r tuple2vec(python::tuple& t){return Vector3r(python::extract<double>(t[0])(),python::extract<double>(t[1])(),python::extract<double>(t[2])());}

struct SpherePack{
	struct Sph{ Vector3r c; Real r; Sph(const Vector3r& _c, Real _r): c(_c), r(_r){};
		python::tuple asTuple() const { return python::make_tuple(c,r); }
	};
	vector<Sph> pack;
	SpherePack(){};
	SpherePack(const python::list& l){ fromList(l); }
	// add single sphere
	void add(const Vector3r& c, Real r){ pack.push_back(Sph(c,r)); }
	// I/O
	void fromList(const python::list& l);
	python::list toList() const;
	void fromFile(const string file);
	void toFile(const string file) const;
	void fromSimulation();
	// spatial characteristics
	Vector3r dim() const {Vector3r mn,mx; aabb(mn,mx); return mx-mn;}
	python::tuple aabb_py() const { Vector3r mn,mx; aabb(mn,mx); return python::make_tuple(mn,mx); }
	void aabb(Vector3r& mn, Vector3r& mx) const {
		Real inf=std::numeric_limits<Real>::infinity(); mn=Vector3r(inf,inf,inf); mx=Vector3r(-inf,-inf,-inf);
		FOREACH(const Sph& s, pack){ Vector3r r(s.r,s.r,s.r); mn=componentMinVector(mn,s.c-r); mx=componentMaxVector(mx,s.c+r);}
	}
	Vector3r midPt() const {Vector3r mn,mx; aabb(mn,mx); return .5*(mn+mx);}
	Real relDensity() const {
		Real sphVol=0; Vector3r dd=dim();
		FOREACH(const Sph& s, pack) sphVol+=pow(s.r,3);
		sphVol*=(4/3.)*Mathr::PI;
		return sphVol/(dd[0]*dd[1]*dd[2]);
	}
	// transformations
	void translate(const Vector3r& shift){ FOREACH(Sph& s, pack) s.c+=shift; }
	void rotate(const Vector3r& axis, Real angle){ Vector3r mid=midPt(); Quaternionr q(axis,angle); FOREACH(Sph& s, pack) s.c=q*(s.c-mid)+mid; }
	void scale(Real scale){ Vector3r mid=midPt(); FOREACH(Sph& s, pack) {s.c=scale*(s.c-mid)+mid; s.r*=abs(scale); } }
	// iteration 
	size_t len() const{ return pack.size(); }
	python::tuple getitem(size_t idx){ if(idx<0 || idx>=pack.size()) throw runtime_error("Index "+lexical_cast<string>(idx)+" out of range 0.."+lexical_cast<string>(pack.size()-1)); return pack[idx].asTuple(); }
	struct iterator{
		const SpherePack& sPack; size_t pos;
		iterator(const SpherePack& _sPack): sPack(_sPack), pos(0){}
		iterator iter(){ return *this;}
		python::tuple next(){
			if(pos==sPack.pack.size()-1){ PyErr_SetNone(PyExc_StopIteration); python::throw_error_already_set(); }
			return sPack.pack[pos++].asTuple();
		}
	};
	SpherePack::iterator getIterator() const{ return SpherePack::iterator(*this);};
};

void SpherePack::fromList(const python::list& l){
	pack.clear();
	size_t len=python::len(l);
	for(size_t i=0; i<len; i++){
		const python::tuple& t=python::extract<python::tuple>(l[i]);
		const Vector3r t0=python::extract<Vector3r>(t[0]);
		pack.push_back(Sph(t0,python::extract<double>(t[1])));
	}
};

python::list SpherePack::toList() const {
	python::list ret;
	FOREACH(const Sph& s, pack) ret.append(python::make_tuple(s.c,s.r));
	return ret;
};

void SpherePack::fromFile(string file) {
	typedef pair<Vector3r,Real> pairVector3rReal;
	vector<pairVector3rReal> ss;
	Vector3r mn,mx;
	ss=Shop::loadSpheresFromFile(file,mn,mx);
	pack.clear();
	FOREACH(const pairVector3rReal& s, ss) pack.push_back(Sph(s.first,s.second));
}

void SpherePack::toFile(const string fname) const {
	ofstream f(fname.c_str());
	if(!f.good()) throw runtime_error("Unable to open file `"+fname+"'");
	FOREACH(const Sph& s, pack) f<<s.c[0]<<" "<<s.c[1]<<" "<<s.c[2]<<" "<<s.r<<endl;
	f.close();
};

void SpherePack::fromSimulation() {
	pack.clear();
	MetaBody* rootBody=Omega::instance().getRootBody().get();
	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
		shared_ptr<InteractingSphere>	intSph=dynamic_pointer_cast<InteractingSphere>(b->interactingGeometry);
		if(!intSph) continue;
		pack.push_back(Sph(b->physicalParameters->se3.position,intSph->radius));
	}
}



BOOST_PYTHON_MODULE(_packSpheres){
	python::class_<SpherePack>("SpherePack","Set of spheres as centers and radii",python::init<python::optional<python::list> >(python::args("list"),"Empty constructor, optionally taking list [ ((cx,cy,cz),r), … ] for initial data." ))
		.def("add",&SpherePack::add,"Add single sphere to packing, given center as 3-tuple and radius")
		.def("toList",&SpherePack::toList,"Return packing data as python list.")
		.def("fromList",&SpherePack::fromList,"Make packing from given list, same format as for constructor. Discards current data.")
		.def("load",&SpherePack::fromFile,"Load packing from external text file (current data will be discarded).")
		.def("save",&SpherePack::toFile,"Save packing to external text file (will be overwritten).")
		.def("fromSimulation",&SpherePack::fromSimulation,"Make packing corresponding to the current simulation. Discards current data.")
		.def("aabb",&SpherePack::aabb_py,"Get axis-aligned bounding box coordinates, as 2 3-tuples.")
		.def("dim",&SpherePack::dim,"Return dimensions of the packing in terms of aabb(), as a 3-tuple.")
		.def("center",&SpherePack::midPt,"Return coordinates of the bounding box center.")
		.def("relDensity",&SpherePack::relDensity,"Relative packing density, measured as sum of spheres' volumes / aabb volume.\n(Sphere overlaps are ignored.)")
		.def("translate",&SpherePack::translate,"Translate all spheres by given vector.")
		.def("rotate",&SpherePack::rotate,"Rotate all spheres around packing center (in terms of aabb()), given axis and angle of the rotation.")
		.def("scale",&SpherePack::scale,"Scale the packing around its center (in terms of aabb()) by given factor (may be negative).")
		.def("__len__",&SpherePack::len,"Get number of spheres in the packing")
		.def("__getitem__",&SpherePack::getitem,"Get entry at given index, as tuple of center and radius.")
		.def("__iter__",&SpherePack::getIterator,"Return iterator over spheres.")
		;
	python::class_<SpherePack::iterator>("SpherePackIterator",python::init<SpherePack::iterator&>())
		.def("__iter__",&SpherePack::iterator::iter)
		.def("next",&SpherePack::iterator::next)
	;
}

