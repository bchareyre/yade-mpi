// © 2009 Václav Šmilauer <eudoxos@arcig.cz>

#include<yade/pkg-dem/SpherePack.hpp>

#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-dem/Shop.hpp>

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

#include<yade/core/Timing.hpp>

// not a serializable in the sense of YADE_PLUGIN

CREATE_LOGGER(SpherePack);

using namespace std;
using namespace boost;

// if we need explicit conversions at a few places
python::tuple vec2tuple(const Vector3r& v){return boost::python::make_tuple(v[0],v[1],v[2]);}
Vector3r tuple2vec(const python::tuple& t){return Vector3r(python::extract<double>(t[0])(),python::extract<double>(t[1])(),python::extract<double>(t[2])());}
Vector3r tuple2vec(python::tuple& t){return Vector3r(python::extract<double>(t[0])(),python::extract<double>(t[1])(),python::extract<double>(t[2])());}

void SpherePack::fromList(const python::list& l){
	pack.clear();
	size_t len=python::len(l);
	for(size_t i=0; i<len; i++){
		const python::tuple& t=python::extract<python::tuple>(l[i]);
		//python::extract<python::tuple> tup(t[0]);
		//if(tup.check()) { pack.push_back(Sph(tuple2vec(tup()),python::extract<double>(t[1]))); continue;}
		python::extract<Vector3r> vec(t[0]);
		if(vec.check()) { pack.push_back(Sph(vec(),python::extract<double>(t[1]))); continue; }
		PyErr_SetString(PyExc_TypeError, "List elements must be (tuple or Vector3, float)!");
		python::throw_error_already_set();
	}
};

python::list SpherePack::toList() const {
	python::list ret;
	FOREACH(const Sph& s, pack) ret.append(python::make_tuple(s.c,s.r));
	return ret;
};

python::list SpherePack::toList_pointsAsTuples() const {
	python::list ret;
	FOREACH(const Sph& s, pack) ret.append(python::make_tuple(vec2tuple(s.c),s.r));
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
	Scene* rootBody=Omega::instance().getScene().get();
	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
		shared_ptr<InteractingSphere>	intSph=dynamic_pointer_cast<InteractingSphere>(b->interactingGeometry);
		if(!intSph) continue;
		pack.push_back(Sph(b->state->pos,intSph->radius));
	}
	if(rootBody->isPeriodic) { cellSize=rootBody->cellMax-rootBody->cellMin; }
}

long SpherePack::makeCloud(Vector3r mn, Vector3r mx, Real rMean, Real rRelFuzz, size_t num, bool periodic){
	static boost::minstd_rand randGen(TimingInfo::getNow(/* get the number even if timing is disabled globally */ true));
	static boost::variate_generator<boost::minstd_rand&, boost::uniform_real<> > rnd(randGen, boost::uniform_real<>(0,1));
	const size_t maxTry=1000;
	Vector3r size=mx-mn;
	if(periodic)(cellSize=size);
	for(size_t i=0; i<num; i++) {
		size_t t;
		for(t=0; t<maxTry; ++t){
			Real r=(rnd()-.5)*rRelFuzz*rMean+rMean; Vector3r c;
			if(!periodic) { for(int axis=0; axis<3; axis++) c[axis]=mn[axis]+r+(size[axis]-2*r)*rnd(); }
			else { for(int axis=0; axis<3; axis++) c[axis]=mn[axis]+size[axis]*rnd(); }
			size_t packSize=pack.size(); bool overlap=false;
			if(!periodic){
				for(size_t j=0; j<packSize; j++){ if(pow(pack[j].r+r,2) >= (pack[j].c-c).SquaredLength()) { overlap=true; break; } }
			} else {
				for(size_t j=0; j<packSize; j++){
					Vector3r dr;
					for(int axis=0; axis<3; axis++) dr[axis]=min(cellWrapRel(c[axis],pack[j].c[axis],pack[j].c[axis]+size[axis]),cellWrapRel(pack[j].c[axis],c[axis],c[axis]+size[axis]));
					if(pow(pack[j].r+r,2)>= dr.SquaredLength()){ overlap=true; break; }
				}
			}
			if(!overlap) { pack.push_back(Sph(c,r)); break; }
		}
		if (t==maxTry) {
			LOG_WARN("Exceeded "<<maxTry<<" tries to insert non-overlapping sphere to packing. Only "<<i<<" spheres was added, although you requested "<<num);
			return i;
		}
	}
	return pack.size();
}

void SpherePack::cellFill(Vector3r vol){
	Vector3<int> count;
	for(int i=0; i<3; i++) count[i]=(int)(ceil(vol[i]/cellSize[i]));
	LOG_DEBUG("Filling volume "<<vol<<" with cell "<<cellSize<<", repeat counts are "<<count);
	cellRepeat(count);
}

void SpherePack::cellRepeat(Vector3<int> count){
	if(cellSize==Vector3r::ZERO){ throw std::runtime_error("cellRepeat cannot be used on non-periodic packing."); }
	if(count[0]<=0 || count[1]<=0 || count[2]<=0){ throw std::invalid_argument("Repeat count components must be positive."); }
	size_t origSize=pack.size();
	pack.reserve(origSize*count[0]*count[1]*count[2]);
	for(int i=0; i<count[0]; i++){
		for(int j=0; j<count[1]; j++){
			for(int k=0; k<count[2]; k++){
				if((i==0) && (j==0) && (k==0)) continue; // original cell
				Vector3r off(cellSize[0]*i,cellSize[1]*j,cellSize[2]*k);
				for(size_t l=0; l<origSize; l++){
					const Sph& s=pack[l]; pack.push_back(Sph(s.c+off,s.r));
				}
			}
		}
	}
	cellSize=Vector3r(cellSize[0]*count[0],cellSize[1]*count[1],cellSize[2]*count[2]);
}
