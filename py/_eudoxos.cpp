#include<yade/lib-pyutil/numpy.hpp>

#include<yade/pkg-dem/ConcretePM.hpp>
#include<boost/python.hpp>
#include<yade/extra/boost_python_len.hpp>
#include<yade/pkg-dem/Shop.hpp>

using namespace boost::python;
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

/* yield surface for the CPM model; this is used only to make yield surface plot from python, for debugging */
Real yieldSigmaTMagnitude(Real sigmaN, int yieldSurfType=0){
	#ifdef CPM_YIELD_SIGMA_T_MAGNITUDE
		/* find first suitable interaction */
		Scene* rootBody=Omega::instance().getScene().get();
		shared_ptr<Interaction> I;
		FOREACH(I, *rootBody->interactions){
			if(I->isReal()) break;
		}
		Real nan=std::numeric_limits<Real>::quiet_NaN();
		if(!I->isReal()) {LOG_ERROR("No real interaction found, returning NaN!"); return nan; }
		CpmPhys* BC=dynamic_cast<CpmPhys*>(I->interactionPhysics.get());
		if(!BC) {LOG_ERROR("Interaction physics is not CpmPhys instance, returning NaN!"); return nan;}
		const Real &omega(BC->omega); const Real& undamagedCohesion(BC->undamagedCohesion); const Real& tanFrictionAngle(BC->tanFrictionAngle);
		const Real& yieldLogSpeed(Law2_Dem3DofGeom_CpmPhys_Cpm::yieldLogSpeed); // const int& yieldSurfType(Law2_Dem3DofGeom_CpmPhys_Cpm::yieldSurfType);
		const Real& yieldEllipseShift(Law2_Dem3DofGeom_CpmPhys_Cpm::yieldEllipseShift);
		return CPM_YIELD_SIGMA_T_MAGNITUDE(sigmaN);
	#else
		LOG_FATAL("CPM model not available in this build.");
		throw;
	#endif
}


// copied from _utils.cpp
Vector3r tuple2vec(const python::tuple& t){return Vector3r(extract<double>(t[0])(),extract<double>(t[1])(),extract<double>(t[2])());}

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

// integrated in CpmStateUpdater
#if 0
/* Compute σxx,σyy,σxy stresses over all spheres, in plane passing through given axis,
	which will be coincident with the y axis in the 2d projection.
	Not sure how much is this function useful... */
std::vector<Vector3r> spiralSphereStresses2d(Real dH_dTheta,const int axis=2){
	Scene* rb=Omega::instance().getScene().get();
	vector<Vector3r> ret(rb->bodies->size(),Vector3r::ZERO);
	int ax1=(axis+1)%3,ax2=(axis+2)%3;
	FOREACH(const shared_ptr<Interaction>& I, *rb->interactions){
		if(!I->isReal()) continue;
		CpmPhys* phys=YADE_CAST<CpmPhys*>(I->interactionPhysics.get());
		Dem3DofGeom* geom=YADE_CAST<Dem3DofGeom*>(I->interactionGeometry.get());
		// get force in this interaction, project it to the plane we need
		Vector3r force=phys->normalForce+phys->shearForce;
		Vector3r y2dIn3d(Vector3r::ZERO); y2dIn3d[axis]=1.;
		Vector3r x2dIn3d(geom->contactPoint); x2dIn3d[axis]=0.; x2dIn3d.Normalize();
		//Vector3r planeNormal=(x2dIn3d).Cross(y2dIn3d); planeNormal.Normalize(); force-=force.Dot(planeNormal)*planeNormal;
		// get contact point in 2d
		Vector2r C; Real theta;
		boost::tie(C[0],C[1],theta)=Shop::spiralProject(geom->contactPoint,dH_dTheta,axis);
		// get force in 2d (ff is already projected to the plane)
		Vector2r _ff(force.Dot(x2dIn3d),force.Dot(y2dIn3d));
		// get positions in 2d (height relative to C, as particle's position could wrap to other spiral period than the contact point, which we don't want here.
		const Vector3r& aa(geom->se31.position); const Vector3r& bb(geom->se32.position);
		Vector2r pos[]={Vector2r(sqrt(pow(aa[ax1],2)+pow(aa[ax2],2)),C[1]+(aa[axis]-geom->contactPoint[axis])),Vector2r(sqrt(pow(bb[ax1],2)+pow(bb[ax2],2)),C[1]+(bb[axis]-geom->contactPoint[axis]))};
		Vector2r ff[]={_ff,-_ff}; body_id_t ids[]={I->getId1(),I->getId2()};
		for(int i=0; i<2; i++){
			// signs of tension/compression along the respective axes
			//Vector2r sgn(pos[i][0]<C[0]?1.:-1.,pos[i][1]<C[1]?1.:-1.);
			int sgn=(C-pos[i]).Dot(ff[i])>0?1:-1; // force in the same direction as vector away from particle: tension (positive)
			ret[ids[i]][0]+=.5*sgn*abs(ff[i][0])/phys->crossSection; ret[ids[i]][1]+=.5*sgn*abs(ff[i][1])/phys->crossSection;
			// divide by the length
			Real torque=(pos[i]-C)[0]*ff[i][1]-(pos[i]-C)[1]*ff[i][0];
			ret[ids[i]][2]+=.5*torque/phys->crossSection;
		}
	}
	return ret;
}
#endif

void particleConfinement(){
	CpmStateUpdater::update();
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
	python::dict ret;
	ret["mass"]=mass;
	ret["vel"]=vel;
	return ret;
}



BOOST_PYTHON_MODULE(_eudoxos){
	import_array();
	def("velocityTowardsAxis",velocityTowardsAxis,velocityTowardsAxis_overloads(args("axisPoint","axisDirection","timeToAxis","subtractDist","perturbation")));
	def("yieldSigmaTMagnitude",yieldSigmaTMagnitude);
	// def("spiralSphereStresses2d",spiralSphereStresses2d,(python::arg("dH_dTheta"),python::arg("axis")=2));
	def("particleConfinement",particleConfinement);
	def("testNumpy",testNumpy);
}

