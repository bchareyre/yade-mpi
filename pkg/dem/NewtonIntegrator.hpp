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
#include<yade/lib/base/Math.hpp>
#include<yade/pkg/common/Callbacks.hpp>
#ifdef YADE_OPENMP
	#include<omp.h>
#endif

/*! An engine that can replace the usual series of engines used for integrating the laws of motion.

 */
class State;
class VelocityBins;

class NewtonIntegrator : public GlobalEngine{
	inline void cundallDamp1st(Vector3r& force, const Vector3r& vel);
	inline void cundallDamp2nd(const Real& dt, const Vector3r& force, const Vector3r& vel, Vector3r& accel);
	bool haveBins;
	inline void leapfrogTranslate(State*, const Body::id_t& id, const Real& dt); // leap-frog translate
	inline void leapfrogSphericalRotate(State*, const Body::id_t& id, const Real& dt); // leap-frog rotate of spherical body
	inline void leapfrogAsphericalRotate(State*, const Body::id_t& id, const Real& dt, const Vector3r& M); // leap-frog rotate of aspherical body
	Quaternionr DotQ(const Vector3r& angVel, const Quaternionr& Q);

	// compute linear and angular acceleration, respecting State::blockedDOFs
	Vector3r computeAccel(const Vector3r& force, const Real& mass, int blockedDOFs);
	Vector3r computeAngAccel(const Vector3r& torque, const Vector3r& inertia, int blockedDOFs);

	void updateEnergy(const shared_ptr<Body>&b, const State* state, const Vector3r& fluctVel, const Vector3r& f, const Vector3r& m);

	// whether the cell has changed from the previous step
	bool cellChanged;
	int homoDeform; // updated from scene at every call; -1 for aperiodic simulations, otherwise equal to scene->cell->homoDeform
	Matrix3r dVelGrad; // dtto

	public:
		// function to save maximum velocity, for the verlet-distance optimization
		void saveMaximaVelocity(const Body::id_t& id, State* state);
		#ifdef YADE_OPENMP
			vector<Real> threadMaxVelocitySq;
		#endif
		/// velocity bins (not used if not created)
		shared_ptr<VelocityBins> velocityBins;
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(NewtonIntegrator,GlobalEngine,"Engine integrating newtonian motion equations.",
		((Real,damping,0.2,,"damping coefficient for Cundall's non viscous damping (see [Chareyre2005]_) [-]"))
		((Real,maxVelocitySq,NaN,,"store square of max. velocity, for informative purposes; computed again at every step. |yupdate|"))
		((bool,exactAsphericalRot,true,,"Enable more exact body rotation integrator for :yref:`aspherical bodies<Body.aspherical>` *only*, using formulation from [Allen1989]_, pg. 89."))
		((Matrix3r,prevVelGrad,Matrix3r::Zero(),,"Store previous velocity gradient (:yref:`Cell::velGrad`) to track acceleration. |yupdate|"))
		#ifdef YADE_BODY_CALLBACK
			((vector<shared_ptr<BodyCallback> >,callbacks,,,"List (std::vector in c++) of :yref:`BodyCallbacks<BodyCallback>` which will be called for each body as it is being processed."))
		#endif
		((Vector3r,prevCellSize,Vector3r(NaN,NaN,NaN),Attr::hidden,"cell size from previous step, used to detect change and find max velocity"))
		((bool,warnNoForceReset,true,,"Warn when forces were not resetted in this step by :yref:`ForceResetter`; this mostly points to :yref:`ForceResetter` being forgotten incidentally and should be disabled only with a good reason."))
		// energy tracking
		((int,nonviscDampIx,-1,(Attr::hidden|Attr::noSave),"Index of the energy dissipated using the non-viscous damping (:yref:`damping<NewtonIntegrator.damping>`)."))
		((bool,kinSplit,false,,"Whether to separately track translational and rotational kinetic energy."))
		((int,kinEnergyIx,-1,(Attr::hidden|Attr::noSave),"Index for kinetic energy in scene->energies."))
		((int,kinEnergyTransIx,-1,(Attr::hidden|Attr::noSave),"Index for translational kinetic energy in scene->energies."))
		((int,kinEnergyRotIx,-1,(Attr::hidden|Attr::noSave),"Index for rotational kinetic energy in scene->energies."))
		,
		/*ctor*/
			#ifdef YADE_OPENMP
				threadMaxVelocitySq.resize(omp_get_max_threads());
			#endif
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(NewtonIntegrator);

