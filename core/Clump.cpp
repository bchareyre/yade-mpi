// (c) 2007-2010 Vaclav Smilauer <eudoxos@arcig.cz> 

#include"Clump.hpp"
#include<core/Scene.hpp>
#include<core/BodyContainer.hpp>
#include<core/State.hpp>
#include<pkg/common/Sphere.hpp>

YADE_PLUGIN((Clump));
CREATE_LOGGER(Clump);

boost::python::dict Clump::members_get(){
  boost::python::dict ret;
	FOREACH(MemberMap::value_type& b, members){
		ret[b.first]=boost::python::make_tuple(b.second.position,b.second.orientation);
	}
	return ret;
}

void Clump::add(const shared_ptr<Body>& clumpBody, const shared_ptr<Body>& subBody){
	Body::id_t subId=subBody->getId();
	const shared_ptr<Clump> clump=YADE_PTR_CAST<Clump>(clumpBody->shape);
	if(clump->members.count(subId)!=0) throw std::invalid_argument(("Body #"+boost::lexical_cast<string>(subId)+" is already part of this clump #"+boost::lexical_cast<string>(clumpBody->id)).c_str());
	if(subBody->isClumpMember()) throw std::invalid_argument(("Body #"+boost::lexical_cast<string>(subId)+" is already a clump member of #"+boost::lexical_cast<string>(subBody->clumpId)).c_str());
	else if(subBody->isClump()){
		const shared_ptr<Clump> subClump=YADE_PTR_CAST<Clump>(subBody->shape);
		FOREACH(const MemberMap::value_type& mm, subClump->members){
			const Body::id_t& memberId=mm.first;
			Scene* scene(Omega::instance().getScene().get());	// get scene
			const shared_ptr<Body>& member=Body::byId(memberId,scene);
			assert(member->isClumpMember());
			member->clumpId=clumpBody->id;
			clump->members[memberId]=Se3r();// meaningful values will be put in by Clump::updateProperties
			//LOG_DEBUG("Added body #"<<memberId->id<<" to clump #"<<clumpBody->id);
		}
		//LOG_DEBUG("Clump #"<<subClump->id<<" will be erased.");// see addToClump() in yadeWrapper.cpp
	}
	else{	// subBody must be a standalone!
		clump->members[subId]=Se3r();// meaningful values will be put in by Clump::updateProperties
		subBody->clumpId=clumpBody->id;
	}
	clumpBody->clumpId=clumpBody->id; // just to make sure
	clumpBody->setBounded(false); // disallow collisions with the clump itself
	if(subBody->isStandalone()){LOG_DEBUG("Added body #"<<subBody->id<<" to clump #"<<clumpBody->id);}
}

void Clump::del(const shared_ptr<Body>& clumpBody, const shared_ptr<Body>& subBody){
	// erase the subBody; removing body that is not part of the clump throws
	const shared_ptr<Clump> clump=YADE_PTR_CAST<Clump>(clumpBody->shape);
	if(clump->members.erase(subBody->id)!=1) throw std::invalid_argument(("Body #"+boost::lexical_cast<string>(subBody->id)+" not part of clump #"+boost::lexical_cast<string>(clumpBody->id)+"; not removing.").c_str());
	subBody->clumpId=Body::ID_NONE;
	LOG_DEBUG("Removed body #"<<subBody->id<<" from clump #"<<clumpBody->id);
}

void Clump::addForceTorqueFromMembers(const State* clumpState, Scene* scene, Vector3r& F, Vector3r& T){
	FOREACH(const MemberMap::value_type& mm, members){
		const Body::id_t& memberId=mm.first; 
		const shared_ptr<Body>& member=Body::byId(memberId,scene); 
		assert(member->isClumpMember()); 
		State* memberState=member->state.get();
		const Vector3r& f=scene->forces.getForce(memberId); 
		const Vector3r& t=scene->forces.getTorque(memberId);
		F+=f;
		T+=t+(memberState->pos-clumpState->pos).cross(f);
	}
}

/*! Clump's se3 will be updated (origin at centroid and axes coincident with principal inertia axes) and subSe3 modified in such a way that members positions in world coordinates will not change.

	Note: velocities and angularVelocities of constituents are zeroed.

	OLD DOCS (will be cleaned up):

	-# Clump::members values and Clump::physicalParameters::se3 are invalid from this point
	-# M=0; S=vector3r(0,0,0); I=zero tensor; (ALL calculations are in world coordinates!)
	-# loop over Clump::members (position x_i, mass m_i, inertia at subBody's centroid I_i) [this loop will be replaced by numerical integration (rasterization) for the intersecting case; the rest will be the same]
		- M+=m_i
		- S+=m_i*x_i (local static moments are zero (centroid)
		- get inertia tensor of subBody in world coordinates, by rotating the principal (local) tensor against subBody->se3->orientation; then translate it to world origin (parallel axes theorem), then I+=I_i_world
	-# clumpPos=S/M
	-# translate aggregate's inertia tensor; parallel axes on I (R=clumpPos): I^c_jk=I'_jk-M*(delta_jk R.R - R_j*R_k) [http://en.wikipedia.org/wiki/Moments_of_inertia#Parallel_axes_theorem]
	-# eigen decomposition of I, get principal inertia and rotation matrix of the clump
	-# se3->orientation=quaternion(rotation_matrix); se3->position=clumpPos
	-#	update subSe3s

*/

void Clump::updateProperties(const shared_ptr<Body>& clumpBody, unsigned int discretization){
	LOG_DEBUG("Updating clump #"<<clumpBody->id<<" parameters");
	const shared_ptr<State> state(clumpBody->state);
	const shared_ptr<Clump> clump(YADE_PTR_CAST<Clump>(clumpBody->shape));
	
	if(clump->members.empty()){ throw std::runtime_error("Clump::updateProperties: clump has zero members."); }
	// trivial case
	if(clump->members.size()==1){
		LOG_DEBUG("Clump of size one will be treated specially.")
		MemberMap::iterator I=clump->members.begin();
		shared_ptr<Body> subBody=Body::byId(I->first);
		//const shared_ptr<RigidBodyParameters>& subRBP(YADE_PTR_CAST<RigidBodyParameters>(subBody->physicalParameters));
		State* subState=subBody->state.get();
		// se3 of the clump as whole is the same as the member's se3
		state->pos=subState->pos;
		state->ori=subState->ori;
		// relative member's se3 is identity
		I->second.position=Vector3r::Zero(); I->second.orientation=Quaternionr::Identity();
		state->inertia=subState->inertia;	
		state->mass=subState->mass;
		state->vel=Vector3r::Zero();
		state->angVel=Vector3r::Zero();
		return;
	}
	//check for intersections:
	bool intersecting = false;
	shared_ptr<Sphere> sph (new Sphere);
	int Sph_Index = sph->getClassIndexStatic();		// get sphere index for checking if bodies are spheres
	if (discretization>0){
		FOREACH(MemberMap::value_type& mm, clump->members){
			const shared_ptr<Body> subBody1=Body::byId(mm.first);
			FOREACH(MemberMap::value_type& mm, clump->members){
				const shared_ptr<Body> subBody2=Body::byId(mm.first);
				if ((subBody1->shape->getClassIndex() ==  Sph_Index) && (subBody2->shape->getClassIndex() ==  Sph_Index) && (subBody1!=subBody2)){//clump members should be spheres
					Vector3r dist = subBody1->state->pos - subBody2->state->pos;
					const Sphere* sphere1 = YADE_CAST<Sphere*> (subBody1->shape.get());
					const Sphere* sphere2 = YADE_CAST<Sphere*> (subBody2->shape.get());
					Real un = (sphere1->radius+sphere2->radius) - dist.norm();
					if (un > 0.001*min(sphere1->radius,sphere2->radius)) {intersecting = true; break;}
				}
			}
			if (intersecting) break;
		}
	}
	/* quantities suffixed by
		g: global (world) coordinates
		s: local subBody's coordinates
		c: local clump coordinates
	*/
	Real M=0; // mass
	Real dens=0;//density
	Vector3r Sg(0,0,0); // static moment, for getting clump's centroid
	Matrix3r Ig(Matrix3r::Zero()), Ic(Matrix3r::Zero()); // tensors of inertia; is upper triangular, zeros instead of symmetric elements
	
	/**
	algorithm for estimation of volumes and inertia tensor from clumps using summation/integration scheme with regular grid spacing
	(some parts copied from woo: http://bazaar.launchpad.net/~eudoxos/woo/trunk/view/head:/pkg/dem/Clump.cpp)
	*/
	if(intersecting){	
		//get boundaries of clump:
		AlignedBox3r aabb;
		FOREACH(MemberMap::value_type& mm, clump->members){
			const shared_ptr<Body> subBody = Body::byId(mm.first);
			if (subBody->shape->getClassIndex() == Sph_Index){//clump member should be a sphere
				const Sphere* sphere = YADE_CAST<Sphere*> (subBody->shape.get());
				aabb.extend(subBody->state->pos + Vector3r::Constant(sphere->radius));
				aabb.extend(subBody->state->pos - Vector3r::Constant(sphere->radius));
			}
		}
		Real rMin=min(aabb.diagonal()[0],min(aabb.diagonal()[1],aabb.diagonal()[2])); 
		
		//get volume and inertia tensor using regular cubic cell array inside bounding box of the clump:
		Real dx = rMin/discretization; 	//edge length of cell
		Real dv = pow(dx,3);		//volume of cell
		long nCells=(aabb.sizes()/dx).prod();
		if(nCells>1e7) LOG_WARN("Clump::updateProperties: Cell array has "<<nCells<<" cells. Integrate inertia may take a while ...");
		Vector3r x;			//position vector (center) of cell
		for(x.x()=aabb.min().x()+dx/2.; x.x()<aabb.max().x(); x.x()+=dx){
			for(x.y()=aabb.min().y()+dx/2.; x.y()<aabb.max().y(); x.y()+=dx){
				for(x.z()=aabb.min().z()+dx/2.; x.z()<aabb.max().z(); x.z()+=dx){
					FOREACH(MemberMap::value_type& mm, clump->members){
						const shared_ptr<Body> subBody = Body::byId(mm.first);
						if (subBody->shape->getClassIndex() == Sph_Index){//clump member should be a sphere
							dens = subBody->material->density;
							const Sphere* sphere = YADE_CAST<Sphere*> (subBody->shape.get());
							if((x-subBody->state->pos).squaredNorm() < pow(sphere->radius,2)){
								Real m = dens*dv;
								M += m;
								Sg += m*x;
								//inertia I = sum_i( mass_i*dist^2 + I_s) )	//steiners theorem
								Ig += m*( x.dot(x)*Matrix3r::Identity()-x*x.transpose())/*dist^2*/+Matrix3r(Vector3r::Constant(dv*pow(dx,2)/6.).asDiagonal())/*I_s/m = d^2: along princial axes of dv; perhaps negligible?*/;
								break;
							}
						}
					}
				}
			}
		}
	} else {//not intersecting
		FOREACH(MemberMap::value_type& mm, clump->members){
			// mm.first is Body::id_t, mm.second is Se3r of that body
			const shared_ptr<Body> subBody=Body::byId(mm.first);
			dens = subBody->material->density;
			if (subBody->shape->getClassIndex() ==  Sph_Index){//clump member should be a sphere
				State* subState=subBody->state.get();
				const Sphere* sphere = YADE_CAST<Sphere*> (subBody->shape.get());
				Real vol = (4./3.)*Mathr::PI*pow(sphere->radius,3.);
				Real m = dens*vol;
				M+=m;
				Sg+=m*subState->pos;
				Ig+=Clump::inertiaTensorTranslate(Vector3r::Constant((2/5.)*m*pow(sphere->radius,2)).asDiagonal(),m,-1.*subState->pos);
			} else { // non-spherical bodies
				State* subState = subBody->state.get();
				const Real& m = subState->mass;
				const Vector3r& inertia = subState->inertia;
				const Vector3r& pos = subState->pos;
				const Quaternionr& ori = subState->ori;
				M += m;
				Sg += m*pos;
				Ig += inertiaTensorTranslate(inertiaTensorRotate(inertia.asDiagonal(),ori),m,-pos);
			}
		}
	}
	assert(M>0); LOG_TRACE("M=\n"<<M<<"\nIg=\n"<<Ig<<"\nSg=\n"<<Sg);
	// clump's centroid
	state->pos=Sg/M;
	// this will calculate translation only, since rotation is zero
	Matrix3r Ic_orientG=inertiaTensorTranslate(Ig, -M /* negative mass means towards centroid */, state->pos); // inertia at clump's centroid but with world orientation
	LOG_TRACE("Ic_orientG=\n"<<Ic_orientG);
	Ic_orientG(1,0)=Ic_orientG(0,1); Ic_orientG(2,0)=Ic_orientG(0,2); Ic_orientG(2,1)=Ic_orientG(1,2); // symmetrize
	Eigen::SelfAdjointEigenSolver<Matrix3r> decomposed(Ic_orientG);
	const Matrix3r& R_g2c(decomposed.eigenvectors());
	// has NaNs for identity matrix??
	LOG_TRACE("R_g2c=\n"<<R_g2c);
	// set quaternion from rotation matrix
	state->ori=Quaternionr(R_g2c); state->ori.normalize();
	state->inertia=decomposed.eigenvalues();
	state->mass=M;
	
	// TODO: these might be calculated from members... but complicated... - someone needs that?!
	state->vel=state->angVel=Vector3r::Zero();
	clumpBody->setAspherical(state->inertia[0]!=state->inertia[1] || state->inertia[0]!=state->inertia[2]);

	// update subBodySe3s; subtract clump orientation (=apply its inverse first) to subBody's orientation
	FOREACH(MemberMap::value_type& I, clump->members){
		shared_ptr<Body> subBody=Body::byId(I.first);
		State* subState=subBody->state.get();
		I.second.orientation=state->ori.conjugate()*subState->ori;
		I.second.position=state->ori.conjugate()*(subState->pos-state->pos);
	}
}



void Clump::updatePropertiesNonSpherical(const shared_ptr<Body>& clumpBody, bool intersecting,shared_ptr<Scene> rb){ //FIXME
//LOG_DEBUG("Updating clump #"<<getId()<<" parameters");
	//LOG_DEBUG("Updating clump #"<<getId()<<" parameters");
	//assert(members.size()>0);
	const shared_ptr<State> state(clumpBody->state);
	const shared_ptr<Clump> clump(YADE_PTR_CAST<Clump>(clumpBody->shape));

	// trivial case
	if(clump->members.size()==1){
		LOG_DEBUG("Clump of size one will be treated specially.")
		MemberMap::iterator I=clump->members.begin();
		shared_ptr<Body> subBody=Body::byId(I->first,rb);
		//const shared_ptr<RigidBodyParameters>& subRBP(YADE_PTR_CAST<RigidBodyParameters>(subBody->physicalParameters));
		State* subState=subBody->state.get();
		// se3 of the clump as whole is the same as the member's se3
		state->pos=subState->pos;
		state->ori=subState->ori;
		// relative member's se3 is identity
		I->second.position=Vector3r::Zero(); I->second.orientation=Quaternionr::Identity();
		state->inertia=subState->inertia;	
		state->mass=subState->mass;
		state->vel=Vector3r::Zero();
		state->angVel=Vector3r::Zero();
		return;
	}

	/* quantities suffixed by
		g: global (world) coordinates
		s: local subBody's coordinates
		c: local clump coordinates
	*/
	double M=0; // mass
	Vector3r Sg(0,0,0); // static moment, for getting clump's centroid
	Matrix3r Ig(Matrix3r::Zero()), Ic(Matrix3r::Zero()); // tensors of inertia; is upper triangular, zeros instead of symmetric elements

	if(intersecting){
		LOG_WARN("Self-intersecting clumps not yet implemented, intersections will be ignored.");
		intersecting=false;
	}

	// begin non-intersecting loop here
	if(!intersecting){
		FOREACH(MemberMap::value_type& I, clump->members){
			// I.first is Body::id_t, I.second is Se3r of that body
			shared_ptr<Body> subBody=Body::byId(I.first,rb);
			State* subState=subBody->state.get();
			M+=subState->mass;
			Sg+=subState->mass*subState->pos;
			// transform from local to global coords
			Quaternionr subState_ori_conjugate=subState->ori.conjugate();
			Matrix3r Imatrix=Matrix3r::Zero(); Imatrix.diagonal()=subState->inertia; 
			// TRWM3MAT(Imatrix); TRWM3QUAT(subRBP_orientation_conjugate);
			Ig+=Clump::inertiaTensorTranslate(Clump::inertiaTensorRotate(Imatrix,subState_ori_conjugate),subState->mass,-1.*subState->pos);
			//TRWM3MAT(Clump::inertiaTensorRotate(Matrix3r(subRBP->inertia),subRBP_orientation_conjugate));
		}
	}
	//TRVAR1(M); TRWM3MAT(Ig); TRWM3VEC(Sg);
	assert(M>0);

	state->pos=Sg/M; // clump's centroid
	// this will calculate translation only, since rotation is zero
	Matrix3r Ic_orientG=Clump::inertiaTensorTranslate(Ig, -M /* negative mass means towards centroid */, state->pos); // inertia at clump's centroid but with world orientation
	//TRWM3MAT(Ic_orientG);

	Matrix3r R_g2c(Matrix3r::Zero()); //rotation matrix
	Ic_orientG(1,0)=Ic_orientG(0,1); Ic_orientG(2,0)=Ic_orientG(0,2); Ic_orientG(2,1)=Ic_orientG(1,2); // symmetrize
	//TRWM3MAT(Ic_orientG);
	matrixEigenDecomposition(Ic_orientG,R_g2c,Ic);
	/*! @bug eigendecomposition might be wrong. see http://article.gmane.org/gmane.science.physics.yade.devel/99 for message. It is worked around below, however.
	*/
	// has NaNs for identity matrix!
	//TRWM3MAT(R_g2c);

	// set quaternion from rotation matrix
	state->ori=Quaternionr(R_g2c); state->ori.normalize();
	// now Ic is diagonal
	state->inertia=Ic.diagonal();
	state->mass=M;


	// this block will be removed once EigenDecomposition works for diagonal matrices
	//#if 1
	//	if(isnan(R_g2c(0,0))||isnan(R_g2c(0,1))||isnan(R_g2c(0,2))||isnan(R_g2c(1,0))||isnan(R_g2c(1,1))||isnan(R_g2c(1,2))||isnan(R_g2c(2,0))||isnan(R_g2c(2,1))||isnan(R_g2c(2,2))){
	//		throw std::logic_error("Clump::updateProperties: NaNs in eigen-decomposition of inertia matrix?!");
	//	}
	//#endif
	//TRWM3VEC(state->inertia);

	// TODO: these might be calculated from members... but complicated... - someone needs that?!
	state->vel=state->angVel=Vector3r::Zero();

	clumpBody->setAspherical(state->inertia[0]!=state->inertia[1] || state->inertia[0]!=state->inertia[2]);

	// update subBodySe3s; subtract clump orientation (=apply its inverse first) to subBody's orientation
	FOREACH(MemberMap::value_type& I, clump->members){
		// now, I->first is Body::id_t, I->second is Se3r of that body
		shared_ptr<Body> subBody=Body::byId(I.first,rb);
		//const shared_ptr<RigidBodyParameters>& subRBP(YADE_PTR_CAST<RigidBodyParameters>(subBody->physicalParameters));
		State* subState=subBody->state.get();
		I.second.orientation=state->ori.conjugate()*subState->ori;
		I.second.position=state->ori.conjugate()*(subState->pos-state->pos);
	}

}

void Clump::updatePropertiesNonSpherical(const shared_ptr<Body>& clumpBody, bool intersecting){ //FIXME
//LOG_DEBUG("Updating clump #"<<getId()<<" parameters");
	//assert(members.size()>0);
	const shared_ptr<State> state(clumpBody->state);
	const shared_ptr<Clump> clump(YADE_PTR_CAST<Clump>(clumpBody->shape));

	// trivial case
	if(clump->members.size()==1){
		LOG_DEBUG("Clump of size one will be treated specially.")
		MemberMap::iterator I=clump->members.begin();
		shared_ptr<Body> subBody=Body::byId(I->first);
		//const shared_ptr<RigidBodyParameters>& subRBP(YADE_PTR_CAST<RigidBodyParameters>(subBody->physicalParameters));
		State* subState=subBody->state.get();
		// se3 of the clump as whole is the same as the member's se3
		state->pos=subState->pos;
		state->ori=subState->ori;
		// relative member's se3 is identity
		I->second.position=Vector3r::Zero(); I->second.orientation=Quaternionr::Identity();
		state->inertia=subState->inertia;	
		state->mass=subState->mass;
		state->vel=Vector3r::Zero();
		state->angVel=Vector3r::Zero();
		return;
	}

	/* quantities suffixed by
		g: global (world) coordinates
		s: local subBody's coordinates
		c: local clump coordinates
	*/
	double M=0; // mass
	Vector3r Sg(0,0,0); // static moment, for getting clump's centroid
	Matrix3r Ig(Matrix3r::Zero()), Ic(Matrix3r::Zero()); // tensors of inertia; is upper triangular, zeros instead of symmetric elements

	if(intersecting){
		LOG_WARN("Self-intersecting clumps not yet implemented, intersections will be ignored.");
		intersecting=false;
	}

	// begin non-intersecting loop here
	if(!intersecting){
		FOREACH(MemberMap::value_type& I, clump->members){
			// I.first is Body::id_t, I.second is Se3r of that body
			shared_ptr<Body> subBody=Body::byId(I.first);
			State* subState=subBody->state.get();
			M+=subState->mass;
			Sg+=subState->mass*subState->pos;
			// transform from local to global coords
			Quaternionr subState_ori_conjugate=subState->ori.conjugate();
			Matrix3r Imatrix=Matrix3r::Zero(); Imatrix.diagonal()=subState->inertia; 
			// TRWM3MAT(Imatrix); TRWM3QUAT(subRBP_orientation_conjugate);
			Ig+=Clump::inertiaTensorTranslate(Clump::inertiaTensorRotate(Imatrix,subState_ori_conjugate),subState->mass,-1.*subState->pos);
			//TRWM3MAT(Clump::inertiaTensorRotate(Matrix3r(subRBP->inertia),subRBP_orientation_conjugate));
		}
	}
	//TRVAR1(M); TRWM3MAT(Ig); TRWM3VEC(Sg);
	assert(M>0);

	state->pos=Sg/M; // clump's centroid
	// this will calculate translation only, since rotation is zero
	Matrix3r Ic_orientG=Clump::inertiaTensorTranslate(Ig, -M /* negative mass means towards centroid */, state->pos); // inertia at clump's centroid but with world orientation
	//TRWM3MAT(Ic_orientG);

	Matrix3r R_g2c(Matrix3r::Zero()); //rotation matrix
	Ic_orientG(1,0)=Ic_orientG(0,1); Ic_orientG(2,0)=Ic_orientG(0,2); Ic_orientG(2,1)=Ic_orientG(1,2); // symmetrize
	//TRWM3MAT(Ic_orientG);
	matrixEigenDecomposition(Ic_orientG,R_g2c,Ic);
	/*! @bug eigendecomposition might be wrong. see http://article.gmane.org/gmane.science.physics.yade.devel/99 for message. It is worked around below, however.
	*/
	// has NaNs for identity matrix!
	//TRWM3MAT(R_g2c);

	// set quaternion from rotation matrix
	state->ori=Quaternionr(R_g2c); state->ori.normalize();
	// now Ic is diagonal
	state->inertia=Ic.diagonal();
	state->mass=M;


	// this block will be removed once EigenDecomposition works for diagonal matrices
	//#if 1
	//	if(isnan(R_g2c(0,0))||isnan(R_g2c(0,1))||isnan(R_g2c(0,2))||isnan(R_g2c(1,0))||isnan(R_g2c(1,1))||isnan(R_g2c(1,2))||isnan(R_g2c(2,0))||isnan(R_g2c(2,1))||isnan(R_g2c(2,2))){
	//		throw std::logic_error("Clump::updateProperties: NaNs in eigen-decomposition of inertia matrix?!");
	//	}
	//#endif
	//TRWM3VEC(state->inertia);

	// TODO: these might be calculated from members... but complicated... - someone needs that?!
	state->vel=state->angVel=Vector3r::Zero();

	clumpBody->setAspherical(state->inertia[0]!=state->inertia[1] || state->inertia[0]!=state->inertia[2]);

	// update subBodySe3s; subtract clump orientation (=apply its inverse first) to subBody's orientation
	FOREACH(MemberMap::value_type& I, clump->members){
		// now, I->first is Body::id_t, I->second is Se3r of that body
		shared_ptr<Body> subBody=Body::byId(I.first);
		//const shared_ptr<RigidBodyParameters>& subRBP(YADE_PTR_CAST<RigidBodyParameters>(subBody->physicalParameters));
		State* subState=subBody->state.get();
		I.second.orientation=state->ori.conjugate()*subState->ori;
		I.second.position=state->ori.conjugate()*(subState->pos-state->pos);
	}


}

void Clump::addNonSpherical(const shared_ptr<Body>& clumpBody, const shared_ptr<Body>& subBody){ //FIXME
	Body::id_t subId=subBody->getId();
	if(subBody->clumpId!=Body::ID_NONE) throw std::invalid_argument(("Body #"+boost::lexical_cast<string>(subId)+" is already in clump #"+boost::lexical_cast<string>(subBody->clumpId)).c_str());
	const shared_ptr<Clump> clump=YADE_PTR_CAST<Clump>(clumpBody->shape);
	if(clump->members.count(subId)!=0) throw std::invalid_argument(("Body #"+boost::lexical_cast<string>(subId)+" is already part of this clump #"+boost::lexical_cast<string>(clumpBody->id)).c_str());
	
	clump->members[subId]=Se3r(); // meaningful values will be put in by Clump::updateProperties
	subBody->clumpId=clumpBody->id;
	clumpBody->clumpId=clumpBody->id; // just to make sure
	clumpBody->setBounded(false); // disallow collisions with the clump itself
	//LOG_DEBUG("Added body #"<<subId<<" to clump #"<<getId());
}
/*! @brief Recalculates inertia tensor of a body after translation away from (default) or towards its centroid.
 *
 * @param I inertia tensor in the original coordinates; it is assumed to be upper-triangular (elements below the diagonal are ignored).
 * @param m mass of the body; if positive, translation is away from the centroid; if negative, towards centroid.
 * @param off offset of the new origin from the original origin
 * @return inertia tensor in the new coordinate system; the matrix is symmetric.
 */
Matrix3r Clump::inertiaTensorTranslate(const Matrix3r& I,const Real m, const Vector3r& off){
	return I+m*(off.dot(off)*Matrix3r::Identity()-off*off.transpose());
}

/*! @brief Recalculate body's inertia tensor in rotated coordinates.
 *
 * @param I inertia tensor in old coordinates
 * @param T rotation matrix from old to new coordinates
 * @return inertia tensor in new coordinates
 */
Matrix3r Clump::inertiaTensorRotate(const Matrix3r& I,const Matrix3r& T){
	/* [http://www.kwon3d.com/theory/moi/triten.html] */
	return T.transpose()*I*T;
}

/*! @brief Recalculate body's inertia tensor in rotated coordinates.
 *
 * @param I inertia tensor in old coordinates
 * @param rot quaternion that describes rotation from old to new coordinates
 * @return inertia tensor in new coordinates
 */
Matrix3r Clump::inertiaTensorRotate(const Matrix3r& I, const Quaternionr& rot){
	Matrix3r T=rot.toRotationMatrix();
	return inertiaTensorRotate(I,T);
}
