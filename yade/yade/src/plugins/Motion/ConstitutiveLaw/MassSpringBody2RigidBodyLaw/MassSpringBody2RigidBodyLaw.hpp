#ifndef __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__
#define __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__

#include "ConstitutiveLaw.hpp"
#include "ActionForce.hpp"
#include "ActionMomentum.hpp"

class MassSpringBody2RigidBodyLaw : public ConstitutiveLaw
{
	public : MassSpringBody2RigidBodyLaw ();
	private : shared_ptr<Action> actionForce;	
	private : shared_ptr<Action> actionMomentum;
	public  : int sdecGroupMask;
	public  : int springGroupMask;

	public : void registerAttributes();

	public : void respondToInteractions(Body* body);
	
	REGISTER_CLASS_NAME(MassSpringBody2RigidBodyLaw);
};

REGISTER_SERIALIZABLE(MassSpringBody2RigidBodyLaw,false);

#endif // __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__
