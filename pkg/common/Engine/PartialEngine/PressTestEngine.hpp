
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
		Real currentVerticalForce, maxVerticalForce, minimalForce, predictedForce, minimalPredictedForce, riseUpPressHigher;
		long int numberIterationAfterDestruction, currentIterationAfterDestruction;
		int pressVelocityForw2Back;
		PressTestEngine(): 
			curentDirection(forward), 
			currentVerticalForce(0), 
			maxVerticalForce(0), 
			minimalForce(0), 
			predictedForce(0), 
			minimalPredictedForce(0),
			riseUpPressHigher(1),
			numberIterationAfterDestruction (0),
			currentIterationAfterDestruction(0), 
			pressVelocityForw2Back(25) {};
		virtual ~PressTestEngine(){};
		virtual void applyCondition(Scene*);
	REGISTER_CLASS_AND_BASE(PressTestEngine,TranslationEngine);
	REGISTER_ATTRIBUTES(TranslationEngine, (numberIterationAfterDestruction) (predictedForce) (riseUpPressHigher));
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(PressTestEngine);
