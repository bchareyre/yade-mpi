/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef STIFFNESS_MATRIX_TIME_STEPPER_HPP
#define STIFFNESS_MATRIX_TIME_STEPPER_HPP

#include<yade/core/TimeStepper.hpp>

class Interaction;
class BodyContainer;
class MacroMicroElasticRelationships;
class MetaBody;
class PhysicalAction;

class GlobalStiffnessTimeStepper : public TimeStepper
{
	private :
		Real		newDt, previousDt;
		bool		computedSomething,
				computedOnce;
		shared_ptr<MacroMicroElasticRelationships> sdecContactModel;
		shared_ptr<PhysicalAction> actionParameterGlobalStiffness;
		int globalStiffnessClassIndex;

		void findTimeStepFromBody(const shared_ptr<Body>& body, MetaBody * ncb);
		void findTimeStepFromInteraction(const shared_ptr<Interaction>& , shared_ptr<BodyContainer>&);

	public :
		int sdecGroupMask; // FIXME - we should find a way to clean groupmask stuff
		//! defaultDt is used as default AND as max value of the timestep
		Real defaultDt;
		//! used as a multiplier on the theoretical critical timestep (compensate some approximations in the computation)
		Real timestepSafetyCoefficient;

		GlobalStiffnessTimeStepper();
		virtual ~GlobalStiffnessTimeStepper();
	
		virtual void computeTimeStep(Body* body);
		virtual bool isActivated();
		DECLARE_LOGGER;


	protected :
		virtual void registerAttributes();

	REGISTER_CLASS_NAME(GlobalStiffnessTimeStepper);
	REGISTER_BASE_CLASS_NAME(TimeStepper);
};

REGISTER_SERIALIZABLE(GlobalStiffnessTimeStepper,false);

#endif //  GLOBAL_STIFFNESS_TIME_STEPPER_HPP

