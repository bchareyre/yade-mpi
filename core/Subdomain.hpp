// (c) 2018 Bruno Chareyre <bruno.chareyre@grenoble-inp.fr>
 
#pragma once

#include<core/Body.hpp>
#include<lib/base/Logging.hpp>
#include<lib/base/Math.hpp>
// #include<core/PartialEngine.hpp>
#include <pkg/common/Aabb.hpp>
#include <pkg/common/Dispatching.hpp>
#include <mpi.h>

class NewtonIntegrator;

class Subdomain: public Shape {
	public:
// 	typedef std::map<Body::id_t,std::vector<Body::id_t> > IntersectionMap; // the lists of bodies from other subdomaines intersecting this one
	//Map fails...
	typedef std::vector< std::vector<Body::id_t> > IntersectionMap; // the lists of bodies from other subdomaines intersecting this one
// 	typedef numpy_boost<Body::id_t,2> testType({1,1}) ;
	boost::python::list intrs_get();
	void intrs_set(const boost::python::list& intrs);
	boost::python::list mIntrs_get();
	void mIntrs_set(const boost::python::list& intrs);
//     	boost::python::dict members_get();
	vector<MPI_Request> mpiReqs;
	
	
	
	// returns pos,vel,angVel,ori of bodies interacting with a given otherDomain
	std::vector<double> getStateValuesFromIds(const vector<Body::id_t>& search) { 
		const shared_ptr<Scene>& scene= Omega::instance().getScene();
		unsigned int N= search.size();
		std::vector<double> res; res.reserve(N*13);
		for (unsigned k=0; k<N; k++) {
			const shared_ptr<State>& s = (*(scene->bodies))[search[k]]->state;
			for (unsigned i=0; i<3; i++) res.push_back(s->pos[i]);
			for (unsigned i=0; i<3; i++) res.push_back(s->vel[i]);
			for (unsigned i=0; i<3; i++) res.push_back(s->angVel[i]);
			for (unsigned i=0; i<4; i++) res.push_back(s->ori.coeffs()[i]);}
		return res;
	}
	
	
	// returns pos,vel,angVel,ori,bounds of bodies interacting with a given otherDomain
	std::vector<double> getStateBoundsValuesFromIds(const vector<Body::id_t>& search) { 
		const shared_ptr<Scene>& scene= Omega::instance().getScene();
		unsigned int N= search.size();
		std::vector<double> res; res.reserve(N*19);
		for (unsigned k=0; k<N; k++) {
			const shared_ptr<Body>& b = (*(scene->bodies))[search[k]];
			const shared_ptr<State>& s = b->state;
			for (unsigned i=0; i<3; i++) res.push_back(s->pos[i]);
			for (unsigned i=0; i<3; i++) res.push_back(s->vel[i]);
			for (unsigned i=0; i<3; i++) res.push_back(s->angVel[i]);
			for (unsigned i=0; i<4; i++) res.push_back(s->ori.coeffs()[i]);
			//HACK
			if  (b->bound){
				for (unsigned i=0; i<3; i++) res.push_back(b->bound->min[i]);
				for (unsigned i=0; i<3; i++) res.push_back(b->bound->max[i]);
			}
			else{
				for (unsigned i=0; i<3; i++) res.push_back(0);
				for (unsigned i=0; i<3; i++) res.push_back(0);
			}
		}
		return res;
	}
	
	std::vector<double> getStateValues(unsigned otherSubdomain) { 
		const shared_ptr<Scene>& scene= Omega::instance().getScene();
		if (scene->subdomain==int(otherSubdomain)) {LOG_ERROR("subdomain cannot interact with itself"); return std::vector<double>();}
		if (otherSubdomain >= intersections.size()) {LOG_ERROR("otherSubdomain exceeds no. of subdomains ("<<otherSubdomain<<" vs. "<<intersections.size()); return std::vector<double>();}
		const vector<Body::id_t>& search = intersections[otherSubdomain];
		return getStateValuesFromIds(search);
	}
	
// 	BROKEN:
// 	boost::python::dict getStateValuesFromIdsAsArray(int otherSubdomain);
	
	
	void setStateValuesFromIds(const vector<Body::id_t>& b_ids, const std::vector<Real>& input) { 
		const shared_ptr<Scene>& scene= Omega::instance().getScene();
		unsigned int N= b_ids.size();
		if ((N*13) != input.size()) LOG_ERROR("size mismatch"<<N*13<<" vs "<<input.size()<< " in "<<scene->subdomain);
		for (unsigned k=0; k<N; k++) {
			const shared_ptr<State>& s = (*(scene->bodies))[b_ids[k]]->state;
			unsigned int idx=k*13;
			s->pos=Vector3r(input[idx],input[idx+1],input[idx+2]);
			s->vel=Vector3r(input[idx+3],input[idx+4],input[idx+5]);
			s->angVel=Vector3r(input[idx+6],input[idx+7],input[idx+8]);
			s->ori=Quaternionr(input[idx+12],input[idx+9],input[idx+10],input[idx+11]);//note q.coeffs() and q(w,x,y,z) take different oredrings
		}
	}
	
	void setStateBoundsValuesFromIds(const vector<Body::id_t>& b_ids, const std::vector<Real>& input) { 
		//HACK
		const shared_ptr<Scene>& scene= Omega::instance().getScene();
		unsigned int N= b_ids.size();
		if ((N*19) != input.size()) LOG_ERROR("size mismatch"<<N*19<<" vs "<<input.size()<< " in "<<scene->subdomain);
		for (unsigned k=0; k<N; k++) {
			const shared_ptr<Body>& b = (*(scene->bodies))[b_ids[k]];
			const shared_ptr<State>& s = b->state;
			unsigned int idx=k*19;
			s->pos=Vector3r(input[idx],input[idx+1],input[idx+2]);
			s->vel=Vector3r(input[idx+3],input[idx+4],input[idx+5]);
			s->angVel=Vector3r(input[idx+6],input[idx+7],input[idx+8]);
			s->ori=Quaternionr(input[idx+12],input[idx+9],input[idx+10],input[idx+11]);//note q.coeffs() and q(w,x,y,z) take different oredrings

			if (!b->bound){
				b->bound = shared_ptr<Bound>(new Bound);
			} else {
				//std::cout << k << " " << input[idx] << " " << input[idx+1] << " " << input[idx+2] << endl;
				std::cout << "set::Hello b<-bound==True" << endl;
			}

			b->bound->min = Vector3r(input[idx+13],input[idx+14],input[idx+15]);
			b->bound->max = Vector3r(input[idx+16],input[idx+17],input[idx+18]);
		}
	}
	
	void setStateValuesFromBuffer(unsigned otherSubdomain) {
		if (mirrorIntersections.size() <= otherSubdomain or stateBuffer.size() <= otherSubdomain) LOG_ERROR("inconsistent size of mirrorIntersections and/or stateBuffer, "<<mirrorIntersections.size()<<" "<<otherSubdomain<<" "<<stateBuffer.size()<<" "<<Omega::instance().getScene()->subdomain);
		setStateValuesFromIds(mirrorIntersections[otherSubdomain],stateBuffer[otherSubdomain]);
	}
	
	void mpiSendStates(/*vector<double> message,*/ unsigned otherSubdomain){
		std::vector<double> vals = getStateValues(otherSubdomain);
		MPI_Send(&vals.front(), vals.size(), MPI_DOUBLE, otherSubdomain, 177, MPI_COMM_WORLD);
	}
	
	void mpiRecvStates(unsigned otherSubdomain){
		if (mirrorIntersections.size() <= otherSubdomain) LOG_ERROR("inconsistent size of mirrorIntersections and/or stateBuffer");
		if (stateBuffer.size() <= otherSubdomain) stateBuffer.resize(otherSubdomain+1);
		const vector<Body::id_t>& b_ids = mirrorIntersections[otherSubdomain];
		unsigned nb = b_ids.size()*13;
		vector<Real>& vals = stateBuffer[otherSubdomain];
		vals.resize(nb);
		int recv_count;
		MPI_Status recv_status;
		MPI_Recv(&vals.front(), nb, MPI_DOUBLE, otherSubdomain, 177, MPI_COMM_WORLD, &recv_status);
		MPI_Get_count(&recv_status,MPI_DOUBLE,&recv_count);
		if (recv_count != int(nb)) LOG_ERROR("length mismatch");
	}
	
	void mpiIrecvStates(unsigned otherSubdomain){
		if (mirrorIntersections.size() <= otherSubdomain) LOG_ERROR("inconsistent size of mirrorIntersections and/or stateBuffer");
		if (stateBuffer.size() <= otherSubdomain) stateBuffer.resize(otherSubdomain+1);
		if (mpiReqs.size() <= otherSubdomain) mpiReqs.resize(otherSubdomain+1);		
		
		const vector<Body::id_t>& b_ids = mirrorIntersections[otherSubdomain];
		unsigned nb = b_ids.size()*13;
		vector<Real>& vals = stateBuffer[otherSubdomain];
		vals.resize(nb);
		
		MPI_Irecv(&vals.front(), nb, MPI_DOUBLE, otherSubdomain, 177, MPI_COMM_WORLD, &mpiReqs[otherSubdomain]);
	}
	
	void mpiWaitReceived(unsigned otherSubdomain){ MPI_Wait(&mpiReqs[otherSubdomain],MPI_STATUS_IGNORE);}
	

	
	//WARNING: precondition: the members bounds have been dispatched already, else we re-use old values. Carefull if subdomain is not at the end of O.bodies
	void setMinMax();
		
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Subdomain,Shape,"The bounding box of a mpi subdomain",
// 		((testType, testArray,testType({0,0}),,""))
		((Real,extraLength,0,,"verlet dist for the subdomain, added to bodies verletDist"))
		((Vector3r,boundsMin,Vector3r(NaN,NaN,NaN),,"min corner of all bboxes of members; differs from effective domain bounds by the extra length (sweepLength)"))
		((Vector3r,boundsMax,Vector3r(NaN,NaN,NaN),,"max corner of all bboxes of members; differs from effective domain bounds by the extra length (sweepLength)"))
		((IntersectionMap,intersections,IntersectionMap(),Attr::hidden,"[will be overridden below]"))
		((IntersectionMap,mirrorIntersections,IntersectionMap(),Attr::hidden,"[will be overridden below]"))
		((vector<Body::id_t>,ids,vector<Body::id_t>(),Attr::hidden,"Ids of owned particles.")) //FIXME
		((vector<vector<Real> >,stateBuffer,vector<vector<Real> >(),(Attr::noSave | Attr::hidden),"container storing data from other subdomains")) 
		,/*ctor*/ createIndex();
		,/*py*/ /*.add_property("members",&Clump::members_get,"Return clump members as {'id1':(relPos,relOri),...}")*/
		.def("setMinMax",&Subdomain::setMinMax,"returns bounding min-max based on members bounds. precondition: the members bounds have been dispatched already, else we re-use old values. Carefull if subdomain is not at the end of O.bodies.")
		.def("getStateValues",&Subdomain::getStateValues,(boost::python::arg("otherDomain")),"returns pos,vel,angVel,ori of bodies interacting with a given otherDomain, based on :yref:`Subdomain.intersections`.")
		.def("getStateValuesFromIds",&Subdomain::getStateValuesFromIds,(boost::python::arg("b_ids")),"returns pos,vel,angVel,ori of listed bodies.")
		.def("setStateValuesFromIds",&Subdomain::setStateValuesFromIds,(boost::python::arg("b_ids"),boost::python::arg("input")),"set pos,vel,angVel,ori from listed body ids and data.")
		.def("getStateBoundsValuesFromIds",&Subdomain::getStateBoundsValuesFromIds,(boost::python::arg("b_ids")),"returns pos,vel,angVel,ori,bounds of listed bodies.")
		.def("setStateBoundsValuesFromIds",&Subdomain::setStateBoundsValuesFromIds,(boost::python::arg("b_ids"),boost::python::arg("input")),"set pos,vel,angVel,ori,bounds from listed body ids and data.")
// 		.def("getStateValuesFromIdsAsArray",&Subdomain::getStateValuesFromIdsAsArray,(boost::python::arg("otherDomain")),"returns 1D numpy array of pos,vel,angVel,ori of bodies interacting with a given otherDomain, based on :yref:`Subdomain.intersections`.")
		.def("setStateValuesFromBuffer",&Subdomain::setStateValuesFromBuffer,(boost::python::arg("subdomain")),"set pos,vel,angVel,ori from state buffer.")
		.def("mpiSendStates",&Subdomain::mpiSendStates,(boost::python::arg("otherSubdomain")),"mpi-send states from current domain to another domain (blocking)")
		.def("mpiRecvStates",&Subdomain::mpiRecvStates,(boost::python::arg("otherSubdomain")),"mpi-recv states from another domain  (blocking)")
		.def("mpiIrecvStates",&Subdomain::mpiIrecvStates,(boost::python::arg("otherSubdomain")),"mpi-Irecv states from another domain  (non-blocking)")
		.def("mpiWaitReceived",&Subdomain::mpiWaitReceived,(boost::python::arg("otherSubdomain")),"mpi-Wait states from another domain (upon return the buffer is set)")		
		.add_property("intersections",&Subdomain::intrs_get,&Subdomain::intrs_set,"lists of bodies from this subdomain intersecting other subdomains. WARNING: only assignement and concatenation allowed")
		.add_property("mirrorIntersections",&Subdomain::mIntrs_get,&Subdomain::mIntrs_set,"lists of bodies from other subdomains intersecting this one. WARNING: only assignement and concatenation allowed")
	);
	DECLARE_LOGGER;
	REGISTER_CLASS_INDEX(Subdomain,Shape);
};
REGISTER_SERIALIZABLE(Subdomain);

class Bo1_Subdomain_Aabb : public BoundFunctor{
	public:
		void go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body*);
	FUNCTOR1D(Subdomain);
	YADE_CLASS_BASE_DOC(Bo1_Subdomain_Aabb,BoundFunctor,"Creates/updates an :yref:`Aabb` of a :yref:`Facet`.");
};
REGISTER_SERIALIZABLE(Bo1_Subdomain_Aabb);


