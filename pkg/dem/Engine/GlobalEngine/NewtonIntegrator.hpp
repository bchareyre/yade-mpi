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
#include<yade/lib-base/Math.hpp>
#include<yade/pkg-common/Callbacks.hpp>
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
	inline void handleClumpMemberAccel(Scene* ncb, const Body::id_t& memberId, State* memberState, State* clumpState);
	inline void handleClumpMemberAngAccel(Scene* ncb, const Body::id_t& memberId, State* memberState, State* clumpState);
	inline void handleClumpMemberTorque(Scene* ncb, const Body::id_t& memberId, State* memberState, State* clumpState, Vector3r& M);
	inline void saveMaximaVelocity(Scene* ncb, const Body::id_t& id, State* state);
	bool haveBins;
	inline void leapfrogTranslate(Scene* ncb, State* state, const Body::id_t& id, const Real& dt); // leap-frog translate
	inline void leapfrogSphericalRotate(Scene* ncb, State* state, const Body::id_t& id, const Real& dt); // leap-frog rotate of spherical body
	inline void leapfrogAsphericalRotate(Scene* ncb, State* state, const Body::id_t& id, const Real& dt, const Vector3r& M); // leap-frog rotate of aspherical body
	Quaternionr DotQ(const Vector3r& angVel, const Quaternionr& Q);
	inline void blockTranslateDOFs(unsigned blockedDOFs, Vector3r& v);
	inline void blockRotateDOFs(unsigned blockedDOFs, Vector3r& v);
	// cell size from previous step, used to detect change, find max velocity and update positions if linearCellResize enabled
	Vector3r prevCellSize;
	// whether the cell has changed from the previous step
	bool cellChanged;

	public:
		#ifdef YADE_OPENMP
			vector<Real> threadMaxVelocitySq;
		#endif
		/// velocity bins (not used if not created)
		shared_ptr<VelocityBins> velocityBins;
		virtual void action();		
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(NewtonIntegrator,GlobalEngine,"Engine integrating newtonian motion equations.",
		((Real,damping,0.2,"damping coefficient for Cundall's non viscous damping (see [Chareyre2005]_) [-]"))
		((Real,maxVelocitySq,NaN,"store square of max. velocity, for informative purposes; computed again at every step. |yupdate|"))
		((bool,exactAsphericalRot,true,"Enable more exact body rotation integrator for aspherical bodies *only*, using formulation from [Allen1989]_, pg. 89."))
		((unsigned short,homotheticCellResize,0,"Enable velocity updates insuring that all bodies move homogeneously while the periodic cell deforms. 0: No update, 1: Reflect on each body the changes in macroscopic velocity field :yref:`Cell::velGrad`, using Δv_i=Δ(grad(v_macro))*x_i. 2: Emulate the Cundall-style equation Δx_i=(grad(v_macro))*x_i, by adding a convective term in the velocity update."))
		((Matrix3r,prevVelGrad,Matrix3r::Zero(),"Store previous velocity gradient (:yref:`Cell::velGrad`) to track acceleration. |yupdate|"))
		((vector<shared_ptr<BodyCallback> >,callbacks,,"List (std::vector in c++) of :yref:`BodyCallbacks<BodyCallback>` which will be called for each body as it is being processed."))
		,
		/*ctor*/
			prevCellSize=Vector3r::Zero();
			#ifdef YADE_OPENMP
				threadMaxVelocitySq.resize(omp_get_max_threads());
			#endif
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(NewtonIntegrator);

