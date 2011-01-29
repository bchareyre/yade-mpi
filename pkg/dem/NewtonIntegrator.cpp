/*************************************************************************
 Copyright (C) 2008 by Bruno Chareyre		                         *
*  bruno.chareyre@hmg.inpg.fr      					 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include<yade/pkg/dem/NewtonIntegrator.hpp>
#include<yade/core/Scene.hpp>
#include<yade/pkg/dem/Clump.hpp>
#include<yade/pkg/common/VelocityBins.hpp>
#include<yade/lib/base/Math.hpp>

YADE_PLUGIN((NewtonIntegrator));
CREATE_LOGGER(NewtonIntegrator);

// 1st order numerical damping
void NewtonIntegrator::cundallDamp1st(Vector3r& force, const Vector3r& vel){
	for(int i=0; i<3; i++) force[i]*=1-damping*Mathr::Sign(force[i]*vel[i]);
}
// 2nd order numerical damping
void NewtonIntegrator::cundallDamp2nd(const Real& dt, const Vector3r& force, const Vector3r& vel, Vector3r& accel){
	for(int i=0; i<3; i++) accel[i]*= 1 - damping*Mathr::Sign ( force[i]*(vel[i] + 0.5*dt*accel[i]) );
}

Vector3r NewtonIntegrator::computeAccel(const Vector3r& force, const Real& mass, int blockedDOFs){
	if(likely(blockedDOFs==0)) return force/mass;
	Vector3r ret(Vector3r::Zero());
	for(int i=0; i<3; i++) if(!(blockedDOFs & State::axisDOF(i,false))) ret[i]+=force[i]/mass;
	return ret;
}
Vector3r NewtonIntegrator::computeAngAccel(const Vector3r& torque, const Vector3r& inertia, int blockedDOFs){
	if(likely(blockedDOFs==0)) return torque.cwise()/inertia;
	Vector3r ret(Vector3r::Zero());
	for(int i=0; i<3; i++) if(!(blockedDOFs & State::axisDOF(i,true))) ret[i]+=torque[i]/inertia[i];
	return ret;
}

void NewtonIntegrator::updateEnergy(const shared_ptr<Body>& b, const State* state, const Vector3r& fluctVel, const Vector3r& f, const Vector3r& m){
	assert(b->isStandalone() || b->isClump());
	// always positive dissipation, by-component: |F_i|*|v_i|*damping*dt (|T_i|*|ω_i|*damping*dt for rotations)
	if(damping!=0.){
		scene->energy->add(fluctVel.cwise().abs().dot(f.cwise().abs())*damping*scene->dt,"nonviscDamp",nonviscDampIx,/*non-incremental*/false);
		// when the aspherical integrator is used, torque is damped instead of ang acceleration; this code is only approximate
		scene->energy->add(state->angVel.cwise().abs().dot(m.cwise().abs())*damping*scene->dt,"nonviscDamp",nonviscDampIx,false);
	}
	// kinetic energy
	Real Etrans=.5*state->mass*fluctVel.squaredNorm();
	Real Erot;
	// rotational terms
	if(b->isAspherical()){
		Matrix3r mI; mI<<state->inertia[0],0,0, 0,state->inertia[1],0, 0,0,state->inertia[2];
		Matrix3r T(state->ori);
		Erot=.5*b->state->angVel.transpose().dot((T.transpose()*mI*T)*b->state->angVel);
	} else { Erot=0.5*state->angVel.dot(state->inertia.cwise()*state->angVel); }
	if(!kinSplit) scene->energy->add(Etrans+Erot,"kinetic",kinEnergyIx,/*non-incremental*/true);
	else{ scene->energy->add(Etrans,"kinTrans",kinEnergyTransIx,true); scene->energy->add(Erot,"kinRot",kinEnergyRotIx,true); }
}

void NewtonIntegrator::saveMaximaVelocity(const Body::id_t& id, State* state){
	if(haveBins) velocityBins->binVelSqUse(id,VelocityBins::getBodyVelSq(state));
	#ifdef YADE_OPENMP
		Real& thrMaxVSq=threadMaxVelocitySq[omp_get_thread_num()]; thrMaxVSq=max(thrMaxVSq,state->vel.squaredNorm());
	#else
		maxVelocitySq=max(maxVelocitySq,state->vel.squaredNorm());
	#endif
}

void NewtonIntegrator::action()
{
	scene->forces.sync();
	if(warnNoForceReset && scene->forces.lastReset<scene->iter) LOG_WARN("O.forces last reset in step "<<scene->forces.lastReset<<", while the current step is "<<scene->iter<<". Did you forget to include ForceResetter in O.engines?");
	const Real& dt=scene->dt;
	homoDeform=(scene->isPeriodic ? scene->cell->homoDeform : -1); // -1 for aperiodic simulations
	dVelGrad=scene->cell->velGrad-prevVelGrad;
	// account for motion of the periodic boundary, if we remember its last position
	// its velocity will count as max velocity of bodies
	// otherwise the collider might not run if only the cell were changing without any particle motion
	// FIXME: will not work for pure shear transformation, which does not change Cell::getSize()
	if(scene->isPeriodic && ((prevCellSize!=scene->cell->getSize())) && /* initial value */!isnan(prevCellSize[0]) ){ cellChanged=true; maxVelocitySq=(prevCellSize-scene->cell->getSize()).squaredNorm()/pow(dt,2); }
	else { maxVelocitySq=0; cellChanged=false; }

	haveBins=(bool)velocityBins;
	if(haveBins) velocityBins->binVelSqInitialize(maxVelocitySq);

	#ifdef YADE_BODY_CALLBACK
		// setup callbacks
		vector<BodyCallback::FuncPtr> callbackPtrs;
		FOREACH(const shared_ptr<BodyCallback>& cb, callbacks){
			cerr<<"<cb="<<cb.get()<<", setting cb->scene="<<scene<<">";
			cb->scene=scene;
			callbackPtrs.push_back(cb->stepInit());
		}
		assert(callbackPtrs.size()==callbacks.size());
		size_t callbacksSize=callbacks.size();
	#endif

	const bool trackEnergy(scene->trackEnergy);
	const bool isPeriodic(scene->isPeriodic);

	#ifdef YADE_OPENMP
		FOREACH(Real& thrMaxVSq, threadMaxVelocitySq) { thrMaxVSq=0; }
	#endif
	YADE_PARALLEL_FOREACH_BODY_BEGIN(const shared_ptr<Body>& b, scene->bodies){
			// clump members are handled inside clumps
			if(unlikely(!b || b->isClumpMember())) continue;

			State* state=b->state.get(); const Body::id_t& id=b->getId();
			Vector3r f=scene->forces.getForce(id), m=scene->forces.getTorque(id);
			#ifdef YADE_DEBUG
				if(isnan(f[0])||isnan(f[1])||isnan(f[2])) throw runtime_error(("NewtonIntegrator: NaN force acting on #"+lexical_cast<string>(id)+".").c_str());
				if(isnan(m[0])||isnan(m[1])||isnan(m[2])) throw runtime_error(("NewtonIntegrator: NaN torque acting on #"+lexical_cast<string>(id)+".").c_str());
				if(state->mass<=0 && ((state->blockedDOFs & State::DOF_XYZ) != State::DOF_XYZ)) throw runtime_error(("NewtonIntegrator: #"+lexical_cast<string>(id)+" has some linear accelerations enabled, but State::mass is non-positive."));
				if(state->inertia.minCoeff()<=0 && ((state->blockedDOFs & State::DOF_RXRYRZ) != State::DOF_RXRYRZ)) throw runtime_error(("NewtonIntegrator: #"+lexical_cast<string>(id)+" has some angular accelerations enabled, but State::inertia contains non-positive terms."));
			#endif

			// fluctuation velocity does not contain meanfield velocity in periodic boundaries
			// in aperiodic boundaries, it is equal to absolute velocity
			Vector3r fluctVel=isPeriodic?scene->cell->bodyFluctuationVel(b->state->pos,b->state->vel):state->vel;

			// numerical damping & kinetic energy
			if(unlikely(trackEnergy)) updateEnergy(b,state,fluctVel,f,m);

			// whether to use aspherical rotation integration for this body; for no accelerations, spherical integrator is "exact" (and faster)
			bool useAspherical=(b->isAspherical() && exactAsphericalRot && state->blockedDOFs!=State::DOF_ALL);

			// for particles not totally blocked, compute accelerations; otherwise, the computations would be useless
			if (state->blockedDOFs!=State::DOF_ALL) {
				// forces
				if(b->isClump()) b->shape->cast<Clump>().addForceTorqueFromMembers(state,scene,f,m);
				// linear acceleration
				Vector3r linAccel=computeAccel(f,state->mass,state->blockedDOFs);
				cundallDamp2nd(dt,f,fluctVel,linAccel);
				state->vel+=dt*linAccel;
				// angular acceleration
				if(!useAspherical){ // uses angular velocity
					Vector3r angAccel=computeAngAccel(m,state->inertia,state->blockedDOFs);
					cundallDamp2nd(dt,m,state->angVel,angAccel);
					state->angVel+=dt*angAccel;
				} else { // uses torque
					for(int i=0; i<3; i++) if(state->blockedDOFs & State::axisDOF(i,true)) m[i]=0; // block DOFs here
					cundallDamp1st(m,state->angVel);
				}
			}

			// update positions from velocities (or torque, for the aspherical integrator)
			leapfrogTranslate(state,id,dt);
			if(!useAspherical) leapfrogSphericalRotate(state,id,dt);
			else leapfrogAsphericalRotate(state,id,dt,m);

			saveMaximaVelocity(id,state);
			// move individual members of the clump, save maxima velocity (for collider stride)
			if(b->isClump()) Clump::moveMembers(b,scene,this);

			#ifdef YADE_BODY_CALLBACK
				// process callbacks
				for(size_t i=0; i<callbacksSize; i++){
					cerr<<"<"<<b->id<<",cb="<<callbacks[i]<<",scene="<<callbacks[i]->scene<<">"; // <<",force="<<callbacks[i]->scene->forces.getForce(b->id)<<">";
					if(callbackPtrs[i]!=NULL) (*(callbackPtrs[i]))(callbacks[i].get(),b.get());
				}
			#endif
	} YADE_PARALLEL_FOREACH_BODY_END();
	#ifdef YADE_OPENMP
		FOREACH(const Real& thrMaxVSq, threadMaxVelocitySq) { maxVelocitySq=max(maxVelocitySq,thrMaxVSq); }
	#endif
	if(haveBins) velocityBins->binVelSqFinalize();
	if(scene->isPeriodic) { prevCellSize=scene->cell->getSize(); prevVelGrad=scene->cell->velGrad; }
}

void NewtonIntegrator::leapfrogTranslate(State* state, const Body::id_t& id, const Real& dt){
	if (scene->forces.getMoveRotUsed()) state->pos+=scene->forces.getMove(id);
	if (homoDeform==Cell::HOMO_VEL || homoDeform==Cell::HOMO_VEL_2ND) {
		// update velocity reflecting changes in the macroscopic velocity field, making the problem homothetic.
		//NOTE : if the velocity is updated before moving the body, it means the current velGrad (i.e. before integration in cell->integrateAndUpdate) will be effective for the current time-step. Is it correct? If not, this velocity update can be moved just after "state->pos += state->vel*dt", meaning the current velocity impulse will be applied at next iteration, after the contact law. (All this assuming the ordering is resetForces->integrateAndUpdate->contactLaw->PeriCompressor->NewtonsLaw. Any other might fool us.)
		//NOTE : dVel defined without wraping the coordinates means bodies out of the (0,0,0) period can move realy fast. It has to be compensated properly in the definition of relative velocities (see Ig2 functors and contact laws).
		//This is the convective term, appearing in the time derivation of Cundall/Thornton expression (dx/dt=velGrad*pos -> d²x/dt²=dvelGrad/dt+velGrad*vel), negligible in many cases but not for high speed large deformations (gaz or turbulent flow).
		if (homoDeform==Cell::HOMO_VEL_2ND) state->vel+=scene->cell->prevVelGrad*state->vel*dt;

		//In all cases, reflect macroscopic (periodic cell) acceleration in the velocity. This is the dominant term in the update in most cases
		Vector3r dVel=dVelGrad*state->pos;
		state->vel+=dVel;
	} else if (homoDeform==Cell::HOMO_POS){
		state->pos+=scene->cell->velGrad*state->pos*dt;
	}
	state->pos+=state->vel*dt;
}

void NewtonIntegrator::leapfrogSphericalRotate(State* state, const Body::id_t& id, const Real& dt )
{
	Vector3r axis = state->angVel;
	if (axis!=Vector3r::Zero()) {//If we have an angular velocity, we make a rotation
		Real angle=axis.norm(); axis/=angle;
		Quaternionr q(AngleAxisr(angle*dt,axis));
		state->ori = q*state->ori;
	}
	if(scene->forces.getMoveRotUsed() && scene->forces.getRot(id)!=Vector3r::Zero()) {
		Vector3r r(scene->forces.getRot(id));
		Real norm=r.norm(); r/=norm;
		Quaternionr q(AngleAxisr(norm,r));
		state->ori=q*state->ori;
	}
	state->ori.normalize();
}

void NewtonIntegrator::leapfrogAsphericalRotate(State* state, const Body::id_t& id, const Real& dt, const Vector3r& M){
	Matrix3r A=state->ori.conjugate().toRotationMatrix(); // rotation matrix from global to local r.f.
	const Vector3r l_n = state->angMom + dt/2 * M; // global angular momentum at time n
	const Vector3r l_b_n = A*l_n; // local angular momentum at time n
	const Vector3r angVel_b_n = l_b_n.cwise()/state->inertia; // local angular velocity at time n
	const Quaternionr dotQ_n=DotQ(angVel_b_n,state->ori); // dQ/dt at time n
	const Quaternionr Q_half = state->ori + dt/2 * dotQ_n; // Q at time n+1/2
	state->angMom+=dt*M; // global angular momentum at time n+1/2
	const Vector3r l_b_half = A*state->angMom; // local angular momentum at time n+1/2
	Vector3r angVel_b_half = l_b_half.cwise()/state->inertia; // local angular velocity at time n+1/2
	const Quaternionr dotQ_half=DotQ(angVel_b_half,Q_half); // dQ/dt at time n+1/2
	state->ori=state->ori+dt*dotQ_half; // Q at time n+1
	state->angVel=state->ori*angVel_b_half; // global angular velocity at time n+1/2

	if(scene->forces.getMoveRotUsed() && scene->forces.getRot(id)!=Vector3r::Zero()) {
		Vector3r r(scene->forces.getRot(id));
		Real norm=r.norm(); r/=norm;
		Quaternionr q(AngleAxisr(norm,r));
		state->ori=q*state->ori;
	}
	state->ori.normalize();
}

// http://www.euclideanspace.com/physics/kinematics/angularvelocity/QuaternionDifferentiation2.pdf
Quaternionr NewtonIntegrator::DotQ(const Vector3r& angVel, const Quaternionr& Q){
	Quaternionr dotQ;
	dotQ.w() = (-Q.x()*angVel[0]-Q.y()*angVel[1]-Q.z()*angVel[2])/2;
	dotQ.x() = ( Q.w()*angVel[0]-Q.z()*angVel[1]+Q.y()*angVel[2])/2;
	dotQ.y() = ( Q.z()*angVel[0]+Q.w()*angVel[1]-Q.x()*angVel[2])/2;
	dotQ.z() = (-Q.y()*angVel[0]+Q.x()*angVel[1]+Q.w()*angVel[2])/2;
	return dotQ;
}
