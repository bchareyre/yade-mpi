#ifndef __EXPLICITMASSSPRINGDYNAMICENGINE_H__
#define __EXPLICITMASSSPRINGDYNAMICENGINE_H__

#include "InteractionLaw.hpp"
#include "Action.hpp"

class ExplicitMassSpringLaw : public InteractionLaw
{
	private : shared_ptr<Action> actionForce;	
	private : shared_ptr<Action> actionMomentum;
	public  : int springGroupMask;

	// construction
	public : ExplicitMassSpringLaw ();

	public : void registerAttributes();

	public : void respondToInteractions(Body* body);
	REGISTER_CLASS_NAME(ExplicitMassSpringLaw);
};

REGISTER_SERIALIZABLE(ExplicitMassSpringLaw,false);

#endif // __EXPLICITMASSSPRINGDYNAMICENGINE_H__
