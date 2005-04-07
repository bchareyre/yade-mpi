#ifndef __SIMPLESPRINGDYNAMICENGINE_H__
#define __SIMPLESPRINGDYNAMICENGINE_H__

#include "ConstitutiveLaw.hpp"
class Action;


class SimpleSpringLaw : public ConstitutiveLaw
{
	private : shared_ptr<Action> actionForce;
	private : shared_ptr<Action> actionMomentum;

	// construction
	public : SimpleSpringLaw ();
	
	public : void registerAttributes();

	public : void calculateForces(Body* body);
	REGISTER_CLASS_NAME(SimpleSpringLaw);
};

REGISTER_SERIALIZABLE(SimpleSpringLaw,false);

#endif // __SIMPLESPRINGDYNAMICENGINE_H__
