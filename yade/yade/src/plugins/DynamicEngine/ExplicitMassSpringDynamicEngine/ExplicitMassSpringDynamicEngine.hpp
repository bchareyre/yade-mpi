#ifndef __EXPLICITMASSSPRINGDYNAMICENGINE_H__
#define __EXPLICITMASSSPRINGDYNAMICENGINE_H__

#include "DynamicEngine.hpp"

class ExplicitMassSpringDynamicEngine : public DynamicEngine
{
	private : std::vector<Vector3r> prevVelocities;
	private : std::vector<Vector3r> forces;
	private : bool first;

	// construction
	public : ExplicitMassSpringDynamicEngine ();
	public : ~ExplicitMassSpringDynamicEngine ();

	public : void afterDeserialization();
	public : void registerAttributes();

	public : void respondToCollisions(Body* body);
	REGISTER_CLASS_NAME(ExplicitMassSpringDynamicEngine);
};

REGISTER_SERIALIZABLE(ExplicitMassSpringDynamicEngine,false);

#endif // __EXPLICITMASSSPRINGDYNAMICENGINE_H__
