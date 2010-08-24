/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/PartialEngine.hpp>
#include<yade/lib-base/Math.hpp>
#include<yade/pkg-dem/TriaxialStressController.hpp>
#include<string>

/** \brief Class for controlling optional initial isotropic compaction and subsequent triaxial test with constant lateral stress and constant axial strain rate.
 *
 * The engine is a state machine with the following states; transitions my be automatic, see below.
 *
 * 1. STATE_ISO_COMPACTION: isotropic compaction (compression) until
 *    the prescribed mean pressue sigmaIsoCompaction is reached and the packing is stable.
 *    The compaction happens either by straining the walls (!internalCompaction)
 *    or by growing size of grains (internalCompaction).
 * 2. STATE_ISO_UNLOADING: isotropic unloading from the previously reached state, until
 *    the mean pressure sigmaLateralConfinement is reached (and stabilizes).
 *    NOTE: this state will be skipped if sigmaLateralConfinement == sigmaIsoCompaction.
 * 3. STATE_TRIAX_LOADING: confined uniaxial compression:
 * 	constant sigmaLateralConfinement is kept at lateral walls (left, right, front, back), while
 * 	top and bottom walls load the packing in their axis (by straining), until the value of epsilonMax
 * 	(deformation along the loading axis) is reached. At this point, the simulation is stopped.
 * 4. STATE_FIXED_POROSITY_COMPACTION: isotropic compaction (compression) until
 *    a chosen porosity value (parameter:fixedPorosity). The six walls move with a chosen translation speed 
 *    (parameter StrainRate).
 * 5. STATE_TRIAX_LIMBO: currently unused, since simulation is hard-stopped in the previous state.
 *
 * Transition from COMPACTION to UNLOADING is done automatically if autoUnload==true;
 * Transition from (UNLOADING to LOADING) or from (COMPACTION to LOADING: if UNLOADING is skipped) is
 *   done automatically if autoCompressionActivation=true;
 * Both autoUnload and autoCompressionActivation are true by default.
 * 
 * NOTE: This engine handles many different manipulations, including some save/reload with attributes modified manually in between. Please don't modify the algorithms, even if they look strange (especially test sequences) without notifying me and getting explicit approval. A typical situation is somebody generates a sample with !autoCompressionActivation and run : he wants a saved simulation at the end. He then reload the saved state, modify some parameters, set autoCompressionActivation=true, and run. He should get the compression test 
 *
 */

class TriaxialCompressionEngine : public TriaxialStressController
{
	private :
		std::string Phase1End;//used to name output files based on current state
				
	public :
		//TriaxialCompressionEngine();
		virtual ~TriaxialCompressionEngine();
		
		// FIXME: current serializer doesn't handle named enum types, this is workaround.
		#define stateNum int
		// should be "enum stateNum {...}" once this is fixed
		enum {STATE_UNINITIALIZED, STATE_ISO_COMPACTION, STATE_ISO_UNLOADING, STATE_TRIAX_LOADING,  STATE_FIXED_POROSITY_COMPACTION, STATE_LIMBO};
		
		void doStateTransition(stateNum nextState);
		#define _STATE_CASE(ST) case ST: return #ST
		string stateName(stateNum st){switch(st){ _STATE_CASE(STATE_UNINITIALIZED);_STATE_CASE(STATE_ISO_COMPACTION);_STATE_CASE(STATE_ISO_UNLOADING);_STATE_CASE(STATE_TRIAX_LOADING);_STATE_CASE(STATE_FIXED_POROSITY_COMPACTION);_STATE_CASE(STATE_LIMBO); default: return "<unknown state>"; } }
		#undef _STATE_CASE

		Vector3r translationAxisx;
		Vector3r translationAxisz;
		//! is isotropicInternalCompactionFinished?
		bool Phase1, saveSimulation, DieCompaction;//FIXME : document DieCompaction
		//! is this the beginning of the simulation, after reading the scene?
		bool firstRun;
		int FinalIterationPhase1, Iteration/*, testEquilibriumInterval*/;//FIXME : what is that?
		
		virtual void action();
		void updateParameters();
		
		///Change physical properties of interactions and/or bodies in the middle of a simulation (change only friction for the moment, complete this function to set cohesion and others before compression test)
		void setContactProperties(Real frictionDegree);
		
		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(
		TriaxialCompressionEngine,TriaxialStressController,
		"The engine is a state machine with the following states; transitions my be automatic, see below.\n\n"
		"#. STATE_ISO_COMPACTION: isotropic compaction (compression) until the prescribed mean pressue sigmaIsoCompaction is reached and the packing is stable. The compaction happens either by straining the walls (!internalCompaction) or by growing size of grains (internalCompaction).\n"
		"#. STATE_ISO_UNLOADING: isotropic unloading from the previously reached state, until the mean pressure sigmaLateralConfinement is reached (and stabilizes).\n\n\t.. note::\n\t\tthis state will be skipped if sigmaLateralConfinement == sigmaIsoCompaction.\n"
		"#.  STATE_TRIAX_LOADING: confined uniaxial compression: constant sigmaLateralConfinement is kept at lateral walls (left, right, front, back), while top and bottom walls load the packing in their axis (by straining), until the value of epsilonMax (deformation along the loading axis) is reached. At this point, the simulation is stopped.\n"
		"#. STATE_FIXED_POROSITY_COMPACTION: isotropic compaction (compression) until a chosen porosity value (parameter:fixedPorosity). The six walls move with a chosen translation speed (parameter StrainRate).\n"
		"#.  STATE_TRIAX_LIMBO: currently unused, since simulation is hard-stopped in the previous state.\n\n"
		"Transition from COMPACTION to UNLOADING is done automatically if autoUnload==true;\n\n Transition from (UNLOADING to LOADING) or from (COMPACTION to LOADING: if UNLOADING is skipped) is done automatically if autoCompressionActivation=true; Both autoUnload and autoCompressionActivation are true by default.\n\n"
		"\n\n.. note::\n\t This engine handles many different manipulations, including some save/reload with attributes modified manually in between. Please don't modify the algorithms, even if they look strange (especially test sequences) without notifying me and getting explicit approval. A typical situation is somebody generates a sample with !autoCompressionActivation and run : he wants a saved simulation at the end. He then reload the saved state, modify some parameters, set autoCompressionActivation=true, and run. He should get the compression test done."
		,
		((Real,strainRate,0,,"target strain rate (./s)"))
		((Real,currentStrainRate,0,,"current strain rate - converging to :yref:`TriaxialCompressionEngine::strainRate` (./s)"))
		((Real,UnbalancedForce,1,,"mean resultant forces divided by mean contact force"))
		((Real,StabilityCriterion,0.001,,"tolerance in terms of :yref:`TriaxialCompressionEngine::UnbalancedForce` to consider the packing is stable"))
		((Vector3r,translationAxis,TriaxialStressController::normal[wall_bottom_id],,"compression axis"))
		((bool,autoCompressionActivation,true,,"Auto-switch from isotropic compaction (or unloading state if sigmaLateralConfinement<sigmaIsoCompaction) to deviatoric loading"))
		((bool,autoUnload,true,,"Auto-switch from isotropic compaction to unloading"))
		((bool,autoStopSimulation,true,,"Stop the simulation when the sample reach STATE_LIMBO, or keep running"))
		((int,testEquilibriumInterval,20,,"interval of checks for transition between phases, higher than 1 saves computation time."))
		((stateNum,currentState,1,,"There are 5 possible states in which TriaxialCompressionEngine can be. See above :yref:`yade.wrapper.TriaxialCompressionEngine` "))
		((stateNum,previousState,1,,"Previous state (used to detect manual changes of the state in .xml)"))
		((Real,sigmaIsoCompaction,1,,"Prescribed isotropic pressure during the compaction phase"))
		((Real,previousSigmaIso,1,,"Previous value of inherited sigma_iso (used to detect manual changes of the confining pressure)"))
		((Real,sigmaLateralConfinement,1,,"Prescribed confining pressure in the deviatoric loading; might be different from :yref:`TriaxialCompressionEngine::sigmaIsoCompaction`"))
		((std::string,Key,"",,"A string appended at the end of all files, use it to name simulations."))
		((bool,noFiles,false,,"If true, no files will be generated (*.xml, *.spheres,...)"))
		((Real,frictionAngleDegree,-1,,"Value of friction assigned just before the deviatoric loading"))
		((Real,epsilonMax,0.5,,"Value of axial deformation for which the loading must stop"))
		((Real,uniaxialEpsilonCurr,1,,"Current value of axial deformation during confined loading (is reference to strain[1])"))
		((Real,fixedPoroCompaction,false,,"A special type of compaction with imposed final porosity :yref:`TriaxialCompressionEngine::fixedPorosity` (WARNING : can give unrealistic results!)"))
		((Real,fixedPorosity,0,,"Value of porosity chosen by the user"))
		((Real,maxStress,0,,"Max value of stress during the simulation (for post-processing)"))
		,
		translationAxisx=Vector3r(1,0,0);
		translationAxisz=Vector3r(0,0,1);
		currentState=STATE_UNINITIALIZED;
		previousState=currentState;
		Phase1End = "Compacted";
		FinalIterationPhase1 = 0;
		Iteration = 0;
		firstRun=true;
		previousSigmaIso=sigma_iso;
		boxVolume=0;
		saveSimulation=false;
		,
	 	.def("setContactProperties",&TriaxialCompressionEngine::setContactProperties,"Assign a new friction angle (degrees) to dynamic bodies and relative interactions")
		)
	DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(TriaxialCompressionEngine);
