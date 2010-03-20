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
class Scene;

class GlobalStiffnessTimeStepper : public TimeStepper
{
	private :
		vector<Vector3r> stiffnesses;
		vector<Vector3r> Rstiffnesses;
		void computeStiffnesses(Scene*);

		Real		newDt;
		bool		computedSomething,
 				computedOnce;
		void findTimeStepFromBody(const shared_ptr<Body>& body, Scene * ncb);
	
	public :
		int sdecGroupMask; // FIXME - to be removed -> not used here but set in preprocessors, removing breaks compilation.
		//! used as a multiplier on the theoretical critical timestep (compensate some approximations in the computation)
		virtual ~GlobalStiffnessTimeStepper();
	
		virtual void computeTimeStep(Scene*);
		virtual bool isActivated();
		YADE_CLASS_BASE_DOC_ATTRS_CTOR(
			GlobalStiffnessTimeStepper,TimeStepper,"An engine assigning the time-step as a fraction of the minimum eigen-period in the problem",
			((Real,defaultDt,1,"used as default AND as max value of the timestep"))
			((Real,previousDt,1,"last computed dt |yupdate|"))
			((Real,timestepSafetyCoefficient,0.8,"safety factor between the minimum eigen-period and the final assigned dt (less than 1))")),
			computedOnce=false;)
		DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(GlobalStiffnessTimeStepper);


