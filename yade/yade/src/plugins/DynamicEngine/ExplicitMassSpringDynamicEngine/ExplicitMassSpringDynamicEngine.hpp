#ifndef __EXPLICITMASSSPRINGDYNAMICENGINE_H__
#define __EXPLICITMASSSPRINGDYNAMICENGINE_H__

#include "DynamicEngine.hpp"

class ExplicitMassSpringDynamicEngine : public DynamicEngine
{
	private : std::vector<Vector3> prevVelocities;
	private : std::vector<Vector3> forces;
	private : bool first;

	// construction
	public : ExplicitMassSpringDynamicEngine ();
	public : ~ExplicitMassSpringDynamicEngine ();

	public : void processAttributes();
	public : void registerAttributes();

	public : void respondToCollisions(Body* body, const std::list<shared_ptr<Interaction> >& interactions);
	REGISTER_CLASS_NAME(ExplicitMassSpringDynamicEngine);
};

REGISTER_SERIALIZABLE(ExplicitMassSpringDynamicEngine,false);

#endif // __EXPLICITMASSSPRINGDYNAMICENGINE_H__
