#ifndef __SIMPLESPRINGDYNAMICENGINE_H__
#define __SIMPLESPRINGDYNAMICENGINE_H__

#include "InteractionLaw.hpp"
class Action;


class SimpleSpringLaw : public InteractionLaw
{
	private : shared_ptr<Action> actionForce;
	private : shared_ptr<Action> actionMomentum;

	// construction
	public : SimpleSpringLaw ();
	
	public : void registerAttributes();

	public : void respondToInteractions(Body* body);
	REGISTER_CLASS_NAME(SimpleSpringLaw);
};

REGISTER_SERIALIZABLE(SimpleSpringLaw,false);

#endif // __SIMPLESPRINGDYNAMICENGINE_H__
