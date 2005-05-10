#ifndef __SIMPLESPRINGDYNAMICENGINE_H__
#define __SIMPLESPRINGDYNAMICENGINE_H__

#include <yade/InteractionSolver.hpp>
class PhysicalAction;


class FrictionLessElasticContactLaw : public InteractionSolver
{
	private : shared_ptr<PhysicalAction> actionForce;
	private : shared_ptr<PhysicalAction> actionMomentum;

	// construction
	public : FrictionLessElasticContactLaw ();
	
	public : void registerAttributes();

	public : void calculateForces(Body* body);
	REGISTER_CLASS_NAME(FrictionLessElasticContactLaw);
};

REGISTER_SERIALIZABLE(FrictionLessElasticContactLaw,false);

#endif // __SIMPLESPRINGDYNAMICENGINE_H__
