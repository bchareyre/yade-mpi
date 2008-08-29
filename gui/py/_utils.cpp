#include<yade/extra/Shop.hpp>
#include<boost/python.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/core/Omega.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<cmath>
using namespace boost::python;

python::tuple vec2tuple(const Vector3r& v){return boost::python::make_tuple(v[0],v[1],v[2]);};

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

python::tuple coordsAndDisplacements(int axis){
	python::list retCoord,retDispl;
	FOREACH(const shared_ptr<Body>&b, *Omega::instance().getRootBody()->bodies){
		retCoord.append(b->physicalParameters->se3.position[axis]);
		retDispl.append(b->physicalParameters->se3.position[axis]-b->physicalParameters->refSe3.position[axis]);
	}
	return python::make_tuple(retCoord,retDispl);
}

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
 * only contacts using SpheresContactGeometry are considered.
 */
python::tuple interactionAnglesHistogram(int axis, size_t bins=20){
	if(axis<0||axis>2) throw invalid_argument("Axis must be from {0,1,2}=x,y,z.");
	Real binStep=Mathr::PI/bins; int axis2=(axis+1)%3, axis3=(axis+2)%3;
	vector<Real> cummProj(bins,0.);
	FOREACH(const shared_ptr<Interaction>& i, *Omega::instance().getRootBody()->transientInteractions){
		shared_ptr<SpheresContactGeometry> scg=dynamic_pointer_cast<SpheresContactGeometry>(i->interactionGeometry);
		if(!scg) continue;
		Vector3r n(scg->normal); n[axis]=0.; Real nLen=n.Length();
		Real theta=acos(n[axis2]/nLen)*(n[axis3]>0?1:-1); if(theta<0) theta+=Mathr::PI;
		int binNo=theta/binStep;
		cummProj[binNo]+=nLen;
	}
	python::list val,binMid;
	for(size_t i=0; i<(size_t)bins; i++){ val.append(cummProj[i]); binMid.append(i*binStep);}
	return python::make_tuple(binMid,val);
}
BOOST_PYTHON_FUNCTION_OVERLOADS(interactionAnglesHistogram_overloads,interactionAnglesHistogram,1,2);


BOOST_PYTHON_MODULE(_utils){
	def("PWaveTimeStep",PWaveTimeStep);
	def("aabbExtrema",aabbExtrema);
	def("negPosExtremeIds",negPosExtremeIds,negPosExtremeIds_overloads(args("axis","distFactor")));
	def("coordsAndDisplacements",coordsAndDisplacements);
	def("setRefSe3",setRefSe3);
	def("interactionAnglesHistogram",interactionAnglesHistogram,interactionAnglesHistogram_overloads(args("axis","bins")));
}


