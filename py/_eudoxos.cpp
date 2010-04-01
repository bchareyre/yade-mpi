#include<yade/lib-pyutil/numpy.hpp>

#include<yade/pkg-dem/ConcretePM.hpp>
#include<boost/python.hpp>
#include<yade/extra/boost_python_len.hpp>
#include<yade/pkg-dem/Shop.hpp>

#ifdef YADE_VTK
	#include<vtkPointLocator.h>
	#include<vtkIdList.h>
	#include<vtkUnstructuredGrid.h>
	#include<vtkPoints.h>
#endif

namespace py = boost::python;
using namespace std;
#ifdef YADE_LOG4CXX
	log4cxx::LoggerPtr logger=log4cxx::Logger::getLogger("yade.eudoxos");
#endif

#ifdef YADE_CPM_FULL_MODEL_AVAILABLE
	#include"../../brefcom-mm.hh"
#endif

# if 0
Real elasticEnergyDensityInAABB(python::tuple Aabb){
	Vector3r bbMin=tuple2vec(python::extract<python::tuple>(Aabb[0])()), bbMax=tuple2vec(python::extract<python::tuple>(Aabb[1])()); Vector3r box=bbMax-bbMin;
	shared_ptr<Scene> rb=Omega::instance().getScene();
	Real E=0;
	FOREACH(const shared_ptr<Interaction>&i, *rb->interactions){
		if(!i->interactionPhysics) continue;
		shared_ptr<CpmPhys> bc=dynamic_pointer_cast<CpmPhys>(i->interactionPhysics); if(!bc) continue;
		const shared_ptr<Body>& b1=Body::byId(i->getId1(),rb), b2=Body::byId(i->getId2(),rb);
		bool isIn1=isInBB(b1->state->pos,bbMin,bbMax), isIn2=isInBB(b2->state->pos,bbMin,bbMax);
		if(!isIn1 && !isIn2) continue;
		Real weight=1.;
		if((!isIn1 && isIn2) || (isIn1 && !isIn2)){
			//shared_ptr<Body> bIn=isIn1?b1:b2, bOut=isIn2?b2:b1;
			Vector3r vIn=(isIn1?b1:b2)->state->pos, vOut=(isIn2?b1:b2)->state->pos;
			#define _WEIGHT_COMPONENT(axis) if(vOut[axis]<bbMin[axis]) weight=min(weight,abs((vOut[axis]-bbMin[axis])/(vOut[axis]-vIn[axis]))); else if(vOut[axis]>bbMax[axis]) weight=min(weight,abs((vOut[axis]-bbMax[axis])/(vOut[axis]-vIn[axis])));
			_WEIGHT_COMPONENT(0); _WEIGHT_COMPONENT(1); _WEIGHT_COMPONENT(2);
			assert(weight>=0 && weight<=1);
		}
		E+=.5*bc->E*bc->crossSection*pow(bc->epsN,2)+.5*bc->G*bc->crossSection*pow(bc->epsT.Length(),2);
	}
	return E/(box[0]*box[1]*box[2]);
}
#endif

// copied from _utils.cpp
Vector3r tuple2vec(const py::tuple& t){return Vector3r(py::extract<double>(t[0])(),py::extract<double>(t[1])(),py::extract<double>(t[2])());}

/*! Set velocity of all dynamic particles so that if their motion were unconstrained,
 * axis given by axisPoint and axisDirection would be reached in timeToAxis
 * (or, point at distance subtractDist from axis would be reached).
 *
 * The code is analogous to AxialGravityEngine and is intended to give initial motion
 * to particles subject to axial compaction to speed up the process. */
void velocityTowardsAxis(const Vector3r& axisPoint, const Vector3r& axisDirection, Real timeToAxis, Real subtractDist=0., Real perturbation=0.1){
	FOREACH(const shared_ptr<Body>&b, *(Omega::instance().getScene()->bodies)){
		if(!b->isDynamic) continue;
		const Vector3r& x0=b->state->pos;
		const Vector3r& x1=axisPoint;
		const Vector3r x2=axisPoint+axisDirection;
		Vector3r closestAxisPoint=(x2-x1) * /* t */ (-(x1-x0).Dot(x2-x1))/((x2-x1).SquaredLength());
		Vector3r toAxis=closestAxisPoint-x0;
		if(subtractDist>0) toAxis*=(toAxis.Length()-subtractDist)/toAxis.Length();
		b->state->vel=toAxis/timeToAxis;
		Vector3r ppDiff=perturbation*(1./sqrt(3.))*Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom())*b->state->vel.Length();
		b->state->vel+=ppDiff;
	}
}
BOOST_PYTHON_FUNCTION_OVERLOADS(velocityTowardsAxis_overloads,velocityTowardsAxis,3,5);

void particleConfinement(){
	CpmStateUpdater csu; csu.update();
}

python::dict testNumpy(){
	Scene* scene=Omega::instance().getScene().get();
	int dim1[]={scene->bodies->size()};
	int dim2[]={scene->bodies->size(),3};
	numpy_boost<Real,1> mass(dim1);
	numpy_boost<Real,2> vel(dim2);
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if(!b) continue;
		mass[b->getId()]=b->state->mass;
		VECTOR3R_TO_NUMPY(vel[b->getId()],b->state->vel);
	}
	py::dict ret;
	ret["mass"]=mass;
	ret["vel"]=vel;
	return ret;
}
#ifdef YADE_VTK


/* Fastly locate interactions withing given distance from a given point. See python docs for details. */
class InteractionLocator{
	// object doing the work, see http://www.vtk.org/doc/release/5.2/html/a01048.html
	vtkPointLocator *locator;
	// used by locator
	vtkUnstructuredGrid* grid;
	vtkPoints* points;
	// maps vtk's id to Interaction objects
	vector<shared_ptr<Interaction> > intrs;
	// axis-aligned bounds of all interactions
	Vector3r mn,mx;
	// count of interactions we hold
	int cnt;
	public:
	InteractionLocator(){
		// traverse all real interactions in the current simulation
		// add them to points, create grid with those points
		// store shared_ptr's to interactions in intrs separately
		Scene* scene=Omega::instance().getScene().get();
		locator=vtkPointLocator::New();
		points=vtkPoints::New();
		FOREACH(const shared_ptr<Interaction>& i, *scene->interactions){
			if(!i->isReal()) continue;
			Dem3DofGeom* ge=dynamic_cast<Dem3DofGeom*>(i->interactionGeometry.get());
			if(!ge) continue;
			const Vector3r& cp(ge->contactPoint);
			int id=points->InsertNextPoint(cp[0],cp[1],cp[2]);
			if(intrs.size()<=(size_t)id){intrs.resize(id+1000);}
			intrs[id]=i;
		}
		double bb[6];
		points->ComputeBounds(); points->GetBounds(bb);
		mn=Vector3r(bb[0],bb[2],bb[4]); mx=Vector3r(bb[1],bb[3],bb[5]);
		cnt=points->GetNumberOfPoints();

		grid=vtkUnstructuredGrid::New();
		grid->SetPoints(points);
		locator->SetDataSet(grid);
		locator->BuildLocator();
	}
	// cleanup
	~InteractionLocator(){ locator->Delete(); points->Delete(); grid->Delete(); }

	py::list intrsWithinDistance(const Vector3r& pt, Real radius){
		vtkIdList *ids=vtkIdList::New();
		locator->FindPointsWithinRadius(radius,(const double*)(&pt),ids);
		int numIds=ids->GetNumberOfIds();
		py::list ret;
		for(int i=0; i<numIds; i++){
			// LOG_TRACE("Add "<<i<<"th id "<<ids->GetId(i));
			ret.append(intrs[ids->GetId(i)]);
		}
		return ret;
	}
	py::tuple getBounds(){ return py::make_tuple(mn,mx);}
	int getCnt(){ return cnt; }
};
#endif

BOOST_PYTHON_MODULE(_eudoxos){
	import_array();
	YADE_SET_DOCSTRING_OPTS;
	py::def("velocityTowardsAxis",velocityTowardsAxis,velocityTowardsAxis_overloads(py::args("axisPoint","axisDirection","timeToAxis","subtractDist","perturbation")));
	// def("spiralSphereStresses2d",spiralSphereStresses2d,(python::arg("dH_dTheta"),python::arg("axis")=2));
	py::def("particleConfinement",particleConfinement);
	py::def("testNumpy",testNumpy);
#ifdef YADE_VTK
	py::class_<InteractionLocator>("InteractionLocator","Locate all (real) interactions in space by their :yref:`contact point<Dem3DofGeom::contactPoint>`. When constructed, all real interactions are spatially indexed (uses vtkPointLocator internally). Use intrsWithinDistance to use those data. \n\n.. note::\n\tData might become inconsistent with real simulation state if simulation is being run between creation of this object and spatial queries.")
		.def("intrsWithinDistance",&InteractionLocator::intrsWithinDistance,((python::arg("point"),python::arg("maxDist"))),"Return list of real interactions that are not further than *maxDist* from *point*.")
		.add_property("bounds",&InteractionLocator::getBounds,"Return coordinates of lower and uppoer corner of axis-aligned abounding box of all interactions")
		.add_property("count",&InteractionLocator::getCnt,"Number of interactions held")
	;
#endif
}
