#ifndef __EXPLICITMASSSPRINGDYNAMICENGINE_H__
#define __EXPLICITMASSSPRINGDYNAMICENGINE_H__

#include <yade/InteractionSolver.hpp>
#include <yade/PhysicalAction.hpp>

class MassSpringLaw : public InteractionSolver
{
	private : shared_ptr<PhysicalAction> actionForce;	
	private : shared_ptr<PhysicalAction> actionMomentum;
	public  : int springGroupMask;

	// construction
	public : MassSpringLaw ();

	public : void registerAttributes();

	public : void calculateForces(Body* body);
	REGISTER_CLASS_NAME(MassSpringLaw);
};

REGISTER_SERIALIZABLE(MassSpringLaw,false);

#endif // __EXPLICITMASSSPRINGDYNAMICENGINE_H__
