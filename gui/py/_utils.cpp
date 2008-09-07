#include<yade/extra/Shop.hpp>
#include<boost/python.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/core/Omega.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<cmath>
using namespace boost::python;

python::tuple vec2tuple(const Vector3r& v){return boost::python::make_tuple(v[0],v[1],v[2]);}
Vector3r tuple2vec(const python::tuple& t){return Vector3r(extract<double>(t[0])(),extract<double>(t[1])(),extract<double>(t[2])());}
bool isInBB(Vector3r p, Vector3r bbMin, Vector3r bbMax){return p[0]>bbMin[0] && p[0]<bbMax[0] && p[1]>bbMin[1] && p[1]<bbMax[1] && p[2]>bbMin[2] && p[2]<bbMax[2];}

/* \todo implement groupMask */
python::tuple aabbExtrema(){
	Real inf=std::numeric_limits<Real>::infinity();
	Vector3r minimum(inf,inf,inf),maximum(-inf,-inf,-inf);
	FOREACH(const shared_ptr<Body>& b, *Omega::instance().getRootBody()->bodies){
		shared_ptr<Sphere> s=dynamic_pointer_cast<Sphere>(b->geometricalModel); if(!s) continue;
		Vector3r rrr(s->radius,s->radius,s->radius);
		minimum=componentMinVector(minimum,b->physicalParameters->se3.position-rrr);
		maximum=componentMaxVector(maximum,b->physicalParameters->se3.position+rrr);
	}
	return python::make_tuple(vec2tuple(minimum),vec2tuple(maximum));
}

python::tuple negPosExtremeIds(int axis, Real distFactor=1.1){
	python::tuple extrema=aabbExtrema();
	Real minCoord=extract<double>(extrema[0][axis])(),maxCoord=extract<double>(extrema[1][axis])();
	python::list minIds,maxIds;
	FOREACH(const shared_ptr<Body>& b, *Omega::instance().getRootBody()->bodies){
		shared_ptr<Sphere> s=dynamic_pointer_cast<Sphere>(b->geometricalModel); if(!s) continue;
		if(b->physicalParameters->se3.position[axis]-s->radius*distFactor<=minCoord) minIds.append(b->getId());
		if(b->physicalParameters->se3.position[axis]+s->radius*distFactor>=maxCoord) maxIds.append(b->getId());
	}
	return python::make_tuple(minIds,maxIds);
}
BOOST_PYTHON_FUNCTION_OVERLOADS(negPosExtremeIds_overloads,negPosExtremeIds,1,2);

python::tuple coordsAndDisplacements(int axis,python::tuple AABB=python::tuple()){
	Vector3r bbMin,bbMax; bool useBB=python::len(AABB)>0;
	if(useBB){bbMin=tuple2vec(extract<python::tuple>(AABB[0])());bbMax=tuple2vec(extract<python::tuple>(AABB[1])());}
	python::list retCoord,retDispl;
	FOREACH(const shared_ptr<Body>&b, *Omega::instance().getRootBody()->bodies){
		if(useBB && !isInBB(b->physicalParameters->se3.position,bbMin,bbMax)) continue;
		retCoord.append(b->physicalParameters->se3.position[axis]);
		retDispl.append(b->physicalParameters->se3.position[axis]-b->physicalParameters->refSe3.position[axis]);
	}
	return python::make_tuple(retCoord,retDispl);
}
BOOST_PYTHON_FUNCTION_OVERLOADS(coordsAndDisplacements_overloads,coordsAndDisplacements,1,2);

void setRefSe3(){
	FOREACH(const shared_ptr<Body>& b, *Omega::instance().getRootBody()->bodies){
		b->physicalParameters->refSe3.position=b->physicalParameters->se3.position;
		b->physicalParameters->refSe3.orientation=b->physicalParameters->se3.orientation;
	}
}

Real PWaveTimeStep(){return Shop::PWaveTimeStep();};


/* return histogram ([bin1Min,bin2Min,…],[value1,value2,…]) from projections of interactions
 * to the plane perpendicular to axis∈[0,1,2]; the number of bins can be specified and they cover
 * the range (0,π), since interactions are bidirecional, hence periodically the same on (π,2π).
 *
 * Only contacts using SpheresContactGeometry are considered.
 * Both bodies must be in the mask (except if mask is 0, when all bodies are considered)
 * If the projection is shorter than minProjLen, it is skipped.
 *
 * @todo implement groupMask
 */
python::tuple interactionAnglesHistogram(int axis, int mask=0, size_t bins=20, Real minProjLen=1e-6){
	if(axis<0||axis>2) throw invalid_argument("Axis must be from {0,1,2}=x,y,z.");
	Real binStep=Mathr::PI/bins; int axis2=(axis+1)%3, axis3=(axis+2)%3;
	vector<Real> cummProj(bins,0.);
	shared_ptr<MetaBody> rb=Omega::instance().getRootBody();
	FOREACH(const shared_ptr<Interaction>& i, *rb->transientInteractions){
		if(!i->isReal) continue;
		if(!(mask==0 || (mask>0 && (Body::byId(i->getId1(),rb)->getGroupMask() & mask) && (Body::byId(i->getId2(),rb)->getGroupMask() & mask)))) continue;
		shared_ptr<SpheresContactGeometry> scg=dynamic_pointer_cast<SpheresContactGeometry>(i->interactionGeometry); if(!scg) continue;
		Vector3r n(scg->normal); n[axis]=0.; Real nLen=n.Length();
		if(nLen<minProjLen) continue; // this interaction is (almost) exactly parallel to our axis; skip that one
		Real theta=acos(n[axis2]/nLen)*(n[axis3]>0?1:-1); if(theta<0) theta+=Mathr::PI;
		int binNo=theta/binStep;
		cummProj[binNo]+=nLen;
	}
	python::list val,binMid;
	for(size_t i=0; i<(size_t)bins; i++){ val.append(cummProj[i]); binMid.append(i*binStep);}
	return python::make_tuple(binMid,val);
}
BOOST_PYTHON_FUNCTION_OVERLOADS(interactionAnglesHistogram_overloads,interactionAnglesHistogram,1,3);


BOOST_PYTHON_MODULE(_utils){
	def("PWaveTimeStep",PWaveTimeStep);
	def("aabbExtrema",aabbExtrema);
	def("negPosExtremeIds",negPosExtremeIds,negPosExtremeIds_overloads(args("axis","distFactor")));
	def("coordsAndDisplacements",coordsAndDisplacements,coordsAndDisplacements_overloads(args("AABB")));
	def("setRefSe3",setRefSe3);
	def("interactionAnglesHistogram",interactionAnglesHistogram,interactionAnglesHistogram_overloads(args("axis","mask","bins")));
}


