/*************************************************************************
 Copyright (C) 2008 by Bruno Chareyre		                         *
*  bruno.chareyre@hmg.inpg.fr      					 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once

#include<yade/core/GlobalEngine.hpp>
#include<yade/core/Interaction.hpp>
#include<Wm3Vector3.h>
#ifdef YADE_OPENMP
	#include<omp.h>
#endif

/*! An engine that can replace the usual series of engines used for integrating the laws of motion.

This engine is faster because it uses less loops and less dispatching 

The result is almost the same as with :
-NewtonsForceLaw
-NewtonsMomentumLaw
-LeapFrogPositionIntegrator
-LeapFrogOrientationIntegrator
-CundallNonViscousForceDamping
-CundallNonViscousMomentumDamping

...but the implementation of damping is slightly different compared to CundallNonViscousForceDamping+CundallNonViscousMomentumDamping. Here, damping is dependent on predicted (undamped) velocity at t+dt/2, while the other engines use velocity at time t.
 
Requirements :
-All dynamic bodies must have physical parameters of type (or inheriting from) BodyMacroParameters
-Physical actions must include forces and moments
 
NOTE: Cundall damping affected dynamic simulation! See examples/dynamic_simulation_tests
 
 */
class State;
class VelocityBins;

class NewtonIntegrator : public GlobalEngine{
	inline void cundallDamp(const Real& dt, const Vector3r& N, const Vector3r& V, Vector3r& A);
	inline void handleClumpMemberAccel(Scene* ncb, const body_id_t& memberId, State* memberState, State* clumpState);
	inline void handleClumpMemberAngAccel(Scene* ncb, const body_id_t& memberId, State* memberState, State* clumpState);
	inline void handleClumpMemberTorque(Scene* ncb, const body_id_t& memberId, State* memberState, State* clumpState, Vector3r& M);
	inline void saveMaximaVelocity(Scene* ncb, const body_id_t& id, State* state);
	bool haveBins;
	inline void leapfrogTranslate(Scene* ncb, State* state, const body_id_t& id, const Real& dt); // leap-frog translate
	inline void leapfrogSphericalRotate(Scene* ncb, State* state, const body_id_t& id, const Real& dt); // leap-frog rotate of spherical body
	inline void leapfrogAsphericalRotate(Scene* ncb, State* state, const body_id_t& id, const Real& dt, const Vector3r& M); // leap-frog rotate of aspherical body
	Quaternionr DotQ(const Vector3r& angVel, const Quaternionr& Q);
	inline void blockTranslateDOFs(unsigned blockedDOFs, Vector3r& v);
	inline void blockRotateDOFs(unsigned blockedDOFs, Vector3r& v);
	// cell size from previous step, used to detect change, find max velocity and update positions if linearCellResize enabled
	Vector3r prevCellSize;
	// whether the cell has changed from the previous step
	bool cellChanged;



	public:
		///damping coefficient for Cundall's non viscous damping
		Real damping;
		/// store square of max. velocity, for informative purposes; computed again at every step
		Real maxVelocitySq;
		/// Enable of the exact aspherical body rotation integrator
		bool exactAsphericalRot;
		//! Enable artificially moving all bodies with the periodic cell, such that its resizes are isotropic. 0: disabled (default), 1: position update, 2: velocity update.
		int homotheticCellResize;
		//! Store transformation increment for the current step (updated automatically)
		Matrix3r cellTrsfInc;

		#ifdef YADE_OPENMP
			vector<Real> threadMaxVelocitySq;
		#endif
		/// velocity bins (not used if not created)
		shared_ptr<VelocityBins> velocityBins;
		virtual void action(Scene *);		
		NewtonIntegrator(): prevCellSize(Vector3r::ZERO),damping(0.2), maxVelocitySq(-1), exactAsphericalRot(false), homotheticCellResize(0){
			#ifdef YADE_OPENMP
				threadMaxVelocitySq.resize(omp_get_max_threads());
			#endif
		}
	YADE_CLASS_BASE_DOC_ATTRS(NewtonIntegrator,GlobalEngine,"Engine integrating newtonian motion equations.",
		((damping,"damping coefficient for Cundall's non viscous damping [-]"))
		((maxVelocitySq,"store square of max. velocity, for informative purposes; computed again at every step. Updated automatically."))
		((exactAsphericalRot,"Enable of the exact aspherical body rotation integrator"))
		((homotheticCellResize,"Enable artificially moving all bodies with the periodic cell, such that its resizes are isotropic. 0: disabled (default), 1: position update, 2: velocity update."))
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(NewtonIntegrator);

