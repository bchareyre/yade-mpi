#ifndef __SIMPLESPRINGDYNAMICENGINE_H__
#define __SIMPLESPRINGDYNAMICENGINE_H__

#include "DynamicEngine.hpp"
class Action;


class SimpleSpringDynamicEngine : public DynamicEngine
{
	private : shared_ptr<Action> actionForce;
	private : shared_ptr<Action> actionMomentum;

	// construction
	public : SimpleSpringDynamicEngine ();
	
	public : void registerAttributes();

	public : void respondToInteractions(Body* body);
	REGISTER_CLASS_NAME(SimpleSpringDynamicEngine);
};

REGISTER_SERIALIZABLE(SimpleSpringDynamicEngine,false);

#endif // __SIMPLESPRINGDYNAMICENGINE_H__
