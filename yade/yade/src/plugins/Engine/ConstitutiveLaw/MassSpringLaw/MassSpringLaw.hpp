#ifndef __EXPLICITMASSSPRINGDYNAMICENGINE_H__
#define __EXPLICITMASSSPRINGDYNAMICENGINE_H__

#include "ConstitutiveLaw.hpp"
#include "ActionParameter.hpp"

class MassSpringLaw : public ConstitutiveLaw
{
	private : shared_ptr<ActionParameter> actionForce;	
	private : shared_ptr<ActionParameter> actionMomentum;
	public  : int springGroupMask;

	// construction
	public : MassSpringLaw ();

	public : void registerAttributes();

	public : void calculateForces(Body* body);
	REGISTER_CLASS_NAME(MassSpringLaw);
};

REGISTER_SERIALIZABLE(MassSpringLaw,false);

#endif // __EXPLICITMASSSPRINGDYNAMICENGINE_H__
