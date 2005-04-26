#ifndef __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__
#define __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__

#include "InteractionSolver.hpp"
#include "ActionParameterForce.hpp"
#include "ActionParameterMomentum.hpp"

class MassSpringBody2RigidBodyLaw : public InteractionSolver
{
	public : MassSpringBody2RigidBodyLaw ();
	private : shared_ptr<ActionParameter> actionForce;	
	private : shared_ptr<ActionParameter> actionMomentum;
	public  : int sdecGroupMask;
	public  : int springGroupMask;

	public : void registerAttributes();

	public : void calculateForces(Body* body);
	
	REGISTER_CLASS_NAME(MassSpringBody2RigidBodyLaw);
};

REGISTER_SERIALIZABLE(MassSpringBody2RigidBodyLaw,false);

#endif // __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__
