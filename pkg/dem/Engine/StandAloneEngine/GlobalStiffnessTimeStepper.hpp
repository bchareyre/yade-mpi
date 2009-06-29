/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/TimeStepper.hpp>


/*! \brief Compute the critical timestep of the leap-frog scheme based on global stiffness of bodies

		
	See usage details in TriaxialTest documentation (TriaxialTest is also a good example of how to use this class)		
 */

class Interaction;
class BodyContainer;
//class MacroMicroElasticRelationships;
class MetaBody;

class GlobalStiffnessTimeStepper : public TimeStepper
{
	private :
		vector<Vector3r> stiffnesses;
		vector<Vector3r> Rstiffnesses;
		void computeStiffnesses(MetaBody*);

		Real		newDt, previousDt;
		bool		computedSomething,
				computedOnce;
		//shared_ptr<MacroMicroElasticRelationships> sdecContactModel;

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
	
		virtual void computeTimeStep(MetaBody*);
		virtual bool isActivated();
		DECLARE_LOGGER;


	protected :
		virtual void registerAttributes();

	REGISTER_CLASS_NAME(GlobalStiffnessTimeStepper);
	REGISTER_BASE_CLASS_NAME(TimeStepper);
};

REGISTER_SERIALIZABLE(GlobalStiffnessTimeStepper);


