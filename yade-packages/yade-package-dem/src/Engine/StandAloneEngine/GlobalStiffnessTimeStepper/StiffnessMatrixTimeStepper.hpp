/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef STIFFNESS_MATRIX_TIME_STEPPER_HPP
#define STIFFNESS_MATRIX_TIME_STEPPER_HPP

#include <yade/yade-core/TimeStepper.hpp>

class Interaction;
class BodyContainer;
class MacroMicroElasticRelationships;
class MetaBody;
class PhysicalAction;

class StiffnessMatrixTimeStepper : public TimeStepper
{
	private :
		Real		newDt;
		bool		computedSomething,
				computedOnce;
		shared_ptr<MacroMicroElasticRelationships> sdecContactModel;
		shared_ptr<PhysicalAction> actionParameterStiffnessMatrix;
		int stiffnessMatrixClassIndex;

		void findTimeStepFromBody(const shared_ptr<Body>& body, MetaBody * ncb);
		void findTimeStepFromInteraction(const shared_ptr<Interaction>& , shared_ptr<BodyContainer>&);

	public :
		int sdecGroupMask; // FIXME - we should find a way to clean groupmask stuff
		//Real defaultDt;

		StiffnessMatrixTimeStepper();
		virtual ~StiffnessMatrixTimeStepper();
	
		virtual void computeTimeStep(Body* body);
		virtual bool isActivated();


	protected :
		virtual void registerAttributes();

	REGISTER_CLASS_NAME(StiffnessMatrixTimeStepper);
	REGISTER_BASE_CLASS_NAME(TimeStepper);
};

REGISTER_SERIALIZABLE(StiffnessMatrixTimeStepper,false);

#endif //  STIFFNESS_MATRIX_TIME_STEPPER_HPP

