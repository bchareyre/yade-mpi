#ifndef __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__
#define __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__

#include "DynamicEngine.hpp"
#include "ActionForce.hpp"
#include "ActionMomentum.hpp"

class MassSpringBody2RigidBodyDynamicEngine : public DynamicEngine
{
	public : MassSpringBody2RigidBodyDynamicEngine ();
	private : shared_ptr<Action> actionForce;	
	private : shared_ptr<Action> actionMomentum;
	public  : int sdecGroup;
	public  : int springGroup;

	public : void registerAttributes();

	public : void respondToInteractions(Body* body);
	
	REGISTER_CLASS_NAME(MassSpringBody2RigidBodyDynamicEngine);
};

REGISTER_SERIALIZABLE(MassSpringBody2RigidBodyDynamicEngine,false);

#endif // __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__
