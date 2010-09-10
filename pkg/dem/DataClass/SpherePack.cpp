// © 2009 Václav Šmilauer <eudoxos@arcig.cz>

#include<yade/pkg-dem/SpherePack.hpp>

#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-dem/Shop.hpp>

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

#include<yade/core/Timing.hpp>

// not a serializable in the sense of YADE_PLUGIN

CREATE_LOGGER(SpherePack);

using namespace std;
using namespace boost;


void SpherePack::fromList(const python::list& l){
	pack.clear();
	size_t len=python::len(l);
	for(size_t i=0; i<len; i++){
		const python::tuple& t=python::extract<python::tuple>(l[i]);
		python::extract<Vector3r> vec(t[0]);
		if(vec.check()) { pack.push_back(Sph(vec(),python::extract<double>(t[1]))); continue; }
		#if 0
			// compatibility block
			python::extract<python::tuple> tup(t[0]);
			if(tup.check()) { pack.push_back(Sph(Vector3r(python::extract<double>(tup[0]),python::extract<double>(tup[1]),python::extract<double>(tup[2]))),python::extract<double>(t[1])); continue; }
		#endif
		PyErr_SetString(PyExc_TypeError, "List elements must be (Vector3, float)!");
		python::throw_error_already_set();
	}
};

python::list SpherePack::toList() const {
	python::list ret;
	FOREACH(const Sph& s, pack) ret.append(python::make_tuple(s.c,s.r));
	return ret;
};
#if 0
python::list SpherePack::toList_pointsAsTuples() const {
	python::list ret;
	FOREACH(const Sph& s, pack) ret.append(python::make_tuple(s.c[0],s.c[1],s.c[2],s.r));
	return ret;
};
#endif


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
	Scene* scene=Omega::instance().getScene().get();
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		shared_ptr<Sphere>	intSph=dynamic_pointer_cast<Sphere>(b->shape);
		if(!intSph) continue;
		pack.push_back(Sph(b->state->pos,intSph->radius));
	}
	if(scene->isPeriodic) { cellSize=scene->cell->getSize(); }
}

long SpherePack::makeCloud(Vector3r mn, Vector3r mx, Real rMean, Real rRelFuzz, int num, bool periodic, Real porosity, const vector<Real>& psdSizes, const vector<Real>& psdCumm, bool distributeMass){
	static boost::minstd_rand randGen(TimingInfo::getNow(/* get the number even if timing is disabled globally */ true));
	static boost::variate_generator<boost::minstd_rand&, boost::uniform_real<Real> > rnd(randGen, boost::uniform_real<Real>(0,1));
	vector<Real> psdRadii; // hold psdSizes as volume if distributeMass is in use, as plain radii (rather than diameters) if not
	vector<Real> psdCumm2; // psdCumm but dimensionally transformed to match mass distribution
	int mode=-1; bool err=false;
	// determine the way we generate radii
	if(rMean>0) mode=RDIST_RMEAN;
	if(porosity>0){
		err=(mode>=0); mode=RDIST_POROSITY;
		if(num<0) throw invalid_argument("SpherePack.makeCloud: when specifying porosity, num must be given as well.");
		Vector3r dimensions=mx-mn; Real volume=dimensions.x()*dimensions.y()*dimensions.z();
		// the term (1+rRelFuzz²) comes from the mean volume for uniform distribution : Vmean = 4/3*pi*Rmean*(1+rRelFuzz²) 
		rMean=pow(volume*(1-porosity)/(Mathr::PI*(4/3.)*(1+rRelFuzz*rRelFuzz)*num),1/3.);
	}
	if(psdSizes.size()>0){
		err=(mode>=0); mode=RDIST_PSD;
		if(psdSizes.size()!=psdCumm.size()) throw invalid_argument(("SpherePack.makeCloud: psdSizes and psdCumm must have same dimensions ("+lexical_cast<string>(psdSizes.size())+"!="+lexical_cast<string>(psdCumm.size())).c_str());
		if(psdSizes.size()<=1) throw invalid_argument("SpherePack.makeCloud: psdSizes must have at least 2 items");
		if((*psdCumm.begin())!=0. && (*psdCumm.rbegin())!=1.) throw invalid_argument("SpherePack.makeCloud: first and last items of psdCumm *must* be exactly 0 and 1.");
		psdRadii.reserve(psdSizes.size());
		for(size_t i=0; i<psdSizes.size(); i++) {
			psdRadii.push_back(/* radius, not diameter */ .5*psdSizes[i]);
			if(distributeMass) psdCumm2.push_back(1-pow(1-psdCumm[i],psdScaleExponent)); // 5/2. is an approximate (fractal) dimension, empirically determined
			LOG_DEBUG(i<<". "<<psdRadii[i]<<", cdf="<<psdCumm[i]<<", cdf2="<<(distributeMass?lexical_cast<string>(psdCumm2[i]):string("--")));
			// check monotonicity
			if(i>0 && (psdSizes[i-1]>psdSizes[i] || psdCumm[i-1]>psdCumm[i])) throw invalid_argument("SpherePack:makeCloud: psdSizes and psdCumm must be both non-decreasing.");
		}
	}
	if(err || mode<0) throw invalid_argument("SpherePack.makeCloud: exactly one of rMean, porosity, psdSizes & psdCumm arguments must be specified.");
	// adjust uniform distribution parameters with distributeMass; rMean has the meaning (dimensionally) of _volume_
	const int maxTry=1000;
	Vector3r size=mx-mn;
	if(periodic)(cellSize=size);
	Real r=0;
	for(int i=0; (i<num) || (num<0); i++) {
		// determine radius of the next sphere we will attempt to place in space
		int t;
		switch(mode){
			case RDIST_RMEAN:
			case RDIST_POROSITY: 
				if(distributeMass) r=pow3Interp(rnd(),rMean*(1-rRelFuzz),rMean*(1+rRelFuzz));
				else r=rMean*(2*(rnd()-.5)*rRelFuzz+1); // uniform distribution in rMean*(1±rRelFuzz)
				break;
			case RDIST_PSD:
				if(distributeMass){
					Real norm;
					int piece=psdGetPiece(rnd(),psdCumm2,norm);
					r=pow3Interp(norm,psdRadii[piece],psdRadii[piece+1]);
				} else {
					Real norm; int piece=psdGetPiece(rnd(),psdCumm,norm);
					r=psdRadii[piece]+norm*(psdRadii[piece+1]-psdRadii[piece]);
				}
		}
		// try to put the sphere into a free spot
		for(t=0; t<maxTry; ++t){
			Vector3r c;
			if(!periodic) { for(int axis=0; axis<3; axis++) c[axis]=mn[axis]+r+(size[axis]-2*r)*rnd(); }
			else { for(int axis=0; axis<3; axis++) c[axis]=mn[axis]+size[axis]*rnd(); }
			size_t packSize=pack.size(); bool overlap=false;
			if(!periodic){
				for(size_t j=0; j<packSize; j++){ if(pow(pack[j].r+r,2) >= (pack[j].c-c).squaredNorm()) { overlap=true; break; } }
			} else {
				for(size_t j=0; j<packSize; j++){
					Vector3r dr;
					for(int axis=0; axis<3; axis++) dr[axis]=min(cellWrapRel(c[axis],pack[j].c[axis],pack[j].c[axis]+size[axis]),cellWrapRel(pack[j].c[axis],c[axis],c[axis]+size[axis]));
					if(pow(pack[j].r+r,2)>= dr.squaredNorm()){ overlap=true; break; }
				}
			}
			if(!overlap) { pack.push_back(Sph(c,r)); break; }
		}
		if (t==maxTry) {
			if(num>0) LOG_WARN("Exceeded "<<maxTry<<" tries to insert non-overlapping sphere to packing. Only "<<i<<" spheres was added, although you requested "<<num);
			return i;
		}
	}
	return pack.size();
}

void SpherePack::cellFill(Vector3r vol){
	Vector3i count;
	for(int i=0; i<3; i++) count[i]=(int)(ceil(vol[i]/cellSize[i]));
	LOG_DEBUG("Filling volume "<<vol<<" with cell "<<cellSize<<", repeat counts are "<<count);
	cellRepeat(count);
}

void SpherePack::cellRepeat(Vector3i count){
	if(cellSize==Vector3r::Zero()){ throw std::runtime_error("cellRepeat cannot be used on non-periodic packing."); }
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

int SpherePack::psdGetPiece(Real x, const vector<Real>& cumm, Real& norm){
	int sz=cumm.size(); int i=0;
	while(i<sz && cumm[i]<=x) i++; // upper interval limit index
	if((i==sz-1) && cumm[i]<=x){ i=sz-2; norm=1.; return i;}
	i--; // lower interval limit intex
	norm=(x-cumm[i])/(cumm[i+1]-cumm[i]);
	//LOG_TRACE("count="<<sz<<", x="<<x<<", piece="<<i<<" in "<<cumm[i]<<"…"<<cumm[i+1]<<", norm="<<norm);
	return i;
}

python::tuple SpherePack::psd(int bins, bool mass) const {
	if(pack.size()==0) return python::make_tuple(python::list(),python::list()); // empty packing
	// find extrema
	Real minD=std::numeric_limits<Real>::infinity(); Real maxD=-minD;
	// volume, but divided by π*4/3
	Real vol=0; long N=pack.size();
	FOREACH(const Sph& s, pack){ maxD=max(2*s.r,maxD); minD=min(2*s.r,minD); vol+=pow(s.r,3); }
	if(minD==maxD){ minD-=.5; maxD+=.5; } // emulates what numpy.histogram does
	// create bins and bin edges
	vector<Real> hist(bins,0); vector<Real> cumm(bins+1,0); /* cummulative values compute from hist at the end */
	vector<Real> edges(bins+1); for(int i=0; i<=bins; i++){ edges[i]=minD+i*(maxD-minD)/bins; }
	// weight each grain by its "volume" relative to overall "volume"
	FOREACH(const Sph& s, pack){
		int bin=int(bins*(2*s.r-minD)/(maxD-minD)); bin=min(bin,bins-1); // to make sure
		if (mass) hist[bin]+=pow(s.r,3)/vol; else hist[bin]+=1./N;
	}
	for(int i=0; i<bins; i++) cumm[i+1]=min(1.,cumm[i]+hist[i]); // cumm[i+1] is OK, cumm.size()==bins+1
	return python::make_tuple(edges,cumm);
}

// New code to include the psd giving few points of it
const float pi = 3.1415926;
long SpherePack::particleSD(Vector3r mn, Vector3r mx, Real rMean, bool periodic, string name, int numSph, const vector<Real>& radii, const vector<Real>& passing){
	Real Vtot=numSph*4./3.*pi*pow(rMean,3.); // total volume of the packing (computed with rMean)
	
	// calculate number of spheres necessary per each radius to match the wanted psd
	// passing has to contain increasing values
	vector<Real> numbers;
	for (size_t i=0; i<radii.size(); i++){
		Real volS=4./3.*pi*pow(radii[i],3.);
		if (i==0) {numbers.push_back(passing[i]/100.*Vtot/volS);}
		else {numbers.push_back((passing[i]-passing[i-1])/100.*Vtot/volS);} // 
		
		cout<<"numbers["<<i<<"] = "<<numbers[i]<<endl;
		cout<<"radii["<<i<<"] = "<<radii[i]<<endl;
		cout<<"vol tot = "<<Vtot<<endl;
		cout<<"v_sphere = "<<volS<<endl;
		cout<<"passing["<<i<<"] = "<<passing[i]<<endl;
	}

	static boost::minstd_rand randGen(TimingInfo::getNow(true));
	static boost::variate_generator<boost::minstd_rand&, boost::uniform_real<> > rnd(randGen, boost::uniform_real<>(0,1));

	const int maxTry=1000;
	Vector3r size=mx-mn;
	if(periodic)(cellSize=size);
	for (int ii=(int)radii.size()-1; ii>=0; ii--){
		Real r=radii[ii]; // select radius
		for(int i=0; i<numbers[ii]; i++) { // place as many spheres as required by the psd for the selected radius into the free spot
			int t;
			for(t=0; t<maxTry; ++t){
				Vector3r c;
				if(!periodic) { for(int axis=0; axis<3; axis++) c[axis]=mn[axis]+r+(size[axis]-2*r)*rnd(); }
				else { for(int axis=0; axis<3; axis++) c[axis]=mn[axis]+size[axis]*rnd(); }
				size_t packSize=pack.size(); bool overlap=false;
				if(!periodic){
					for(size_t j=0; j<packSize; j++){ if(pow(pack[j].r+r,2) >= (pack[j].c-c).squaredNorm()) { overlap=true; break; } }
				} else {
					for(size_t j=0; j<packSize; j++){
						Vector3r dr;
						for(int axis=0; axis<3; axis++) dr[axis]=min(cellWrapRel(c[axis],pack[j].c[axis],pack[j].c[axis]+size[axis]),cellWrapRel(pack[j].c[axis],c[axis],c[axis]+size[axis]));
						if(pow(pack[j].r+r,2)>= dr.squaredNorm()){ overlap=true; break; }
					}
				}
				if(!overlap) { pack.push_back(Sph(c,r)); break; }
			}
			if (t==maxTry) {
				if(numbers[ii]>0) LOG_WARN("Exceeded "<<maxTry<<" tries to insert non-overlapping sphere to packing. Only "<<i<<" spheres was added, although you requested "<<numbers[ii]<<" with radius"<<radii[ii]);
				return i;
			}
		}
	}
	return pack.size();
}


