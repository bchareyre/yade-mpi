
#pragma once
#include<yade/pkg-common/TranslationEngine.hpp>
#include<yade/core/PartialEngine.hpp>

class PressTestEngine: public TranslationEngine{
	/**
	 * This class simulates the simple press work
	 * When the press "cracks" the solid brittle material,
	 * it returns back to the initial position 
	 * and stops the simulation loop.
	 */ 
	public:
		enum motionDirection {forward, backward, stop};
		motionDirection curentDirection;
		Real currentVerticalForce, maxVerticalForce, minimalForce, minimalPredictedForce;
		long int currentIterationAfterDestruction;
		int pressVelocityForw2Back;
		virtual ~PressTestEngine(){};
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(PressTestEngine,TranslationEngine,"This class simulates the simple press work When the press cracks the solid brittle material, it returns back to the initial position and stops the simulation loop.",((long int,numberIterationAfterDestruction,0,"The number of iterations, which will be carry out after destruction [-]"))
		((Real,predictedForce,0,"The minimal force, after what the engine will look for a destruction [N]"))
		((long int,riseUpPressHigher,1,"After destruction press rises up. This is the relationship between initial press velocity and velocity for going *back* [-]")),
		curentDirection=forward;currentVerticalForce=0;maxVerticalForce=0;minimalForce=0;minimalPredictedForce=0;currentIterationAfterDestruction=0;pressVelocityForw2Back=25;
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(PressTestEngine);
