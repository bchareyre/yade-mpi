#ifndef __EXPLICITMASSSPRINGDYNAMICENGINE_H__
#define __EXPLICITMASSSPRINGDYNAMICENGINE_H__

#include "DynamicEngine.hpp"
#include "Action.hpp"

class ExplicitMassSpringDynamicEngine : public DynamicEngine
{
	private : shared_ptr<Action> actionForce;	
	private : shared_ptr<Action> actionMomentum;
	private : bool first;

	// construction
	public : ExplicitMassSpringDynamicEngine ();
	public : ~ExplicitMassSpringDynamicEngine ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : void respondToInteractions(Body* body);
	REGISTER_CLASS_NAME(ExplicitMassSpringDynamicEngine);
};

REGISTER_SERIALIZABLE(ExplicitMassSpringDynamicEngine,false);

#endif // __EXPLICITMASSSPRINGDYNAMICENGINE_H__
