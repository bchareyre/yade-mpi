#include<yade/extra/Shop.hpp>
#include<boost/python.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/core/Omega.hpp>
#include<yade/pkg-common/Sphere.hpp>
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

BOOST_PYTHON_MODULE(_utils){
	def("PWaveTimeStep",PWaveTimeStep);
	def("aabbExtrema",aabbExtrema);
	def("negPosExtremeIds",negPosExtremeIds,negPosExtremeIds_overloads(args("axis","distFactor")));
	def("coordsAndDisplacements",coordsAndDisplacements);
	def("setRefSe3",setRefSe3);
}


