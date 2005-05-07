#ifndef __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__
#define __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__

#include "InteractionSolver.hpp"
#include "Force.hpp"
#include "Momentum.hpp"

class MassSpringBody2RigidBodyLaw : public InteractionSolver
{
	public : MassSpringBody2RigidBodyLaw ();
	private : shared_ptr<PhysicalAction> actionForce;	
	private : shared_ptr<PhysicalAction> actionMomentum;
	public  : int sdecGroupMask;
	public  : int springGroupMask;

	public : void registerAttributes();

	public : void calculateForces(Body* body);
	
	REGISTER_CLASS_NAME(MassSpringBody2RigidBodyLaw);
};

REGISTER_SERIALIZABLE(MassSpringBody2RigidBodyLaw,false);

#endif // __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__
