/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#ifndef TRIAXIALCOMPRESSIONENGINE_HPP
#define TRIAXIALCOMPRESSIONENGINE_HPP

#include<yade/core/DeusExMachina.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>
#include "TriaxialStressController.hpp"
#include <string>

// FIXME: current serializer doesn't handle named enum types, this is workaround.
#define stateNum int

/*! \brief Isotropic compression + uniaxial compression test

	detailed description...
 */

class PhysicalAction;

/** \brief Class for controlling optional initial isotropic compaction and subsequent triaxial stress test with hydrostatic confinement.
 *
 *
 * 
 */
class TriaxialCompressionEngine : public TriaxialStressController
{
	private :
		shared_ptr<PhysicalAction> actionForce;
				
	public :
		TriaxialCompressionEngine();
		virtual ~TriaxialCompressionEngine();

		enum {STATE_ISO_COMPACTION, STATE_ISO_UNLOADING, STATE_TRIAX_LOADING, STATE_LIMBO}; 
		stateNum currentState;
		//const char* stateNames[]={"istropic_compression","isotropic_unloading","triaxial_loading"};
		void doStateTransition(stateNum nextState);
		
		//! Strain velocity (./s)
		Real strainRate;
		Real currentStrainRate;
		//! Max ratio of resultant forces on mean contact force
		Real UnbalancedForce;
		//! Value of UnbalancedForce for which the system is considered stable
		Real StabilityCriterion;
		//! Previous value of inherited sigma_iso (used to detect manual changes of the confining pressure)
		Real previousSigmaIso;
		//! Previous state (used to detect manual changes of the state in .xml)
		stateNum previousState;
		//Vector3r strain;
		Vector3r translationAxis;
		//! is isotropicInternalCompactionFinished?
		bool Phase1, saveSimulation;
		//! is this the beginning of the simulation, after reading the scene?
		bool firstRun;
		int FinalIterationPhase1, Iteration, testEquilibriumInterval;
		std::string Phase1End; //,Phase2End;
		// //! Is uniaxial compression currently activated?
		// bool compressionActivated;
		//! Auto-switch between isotropic and uniaxial compression?
		bool autoCompressionActivation;
				
		virtual void applyCondition(Body * body);
		void updateParameters(Body * body);


		DECLARE_LOGGER;
		

	protected :
		virtual void postProcessAttributes(bool);
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(TriaxialCompressionEngine);
	REGISTER_BASE_CLASS_NAME(TriaxialStressController);
};

REGISTER_SERIALIZABLE(TriaxialCompressionEngine,false);

#endif //  TRIAXIALCOMPRESSIONENGINE_HPP

