#ifndef __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__
#define __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__

#include "DynamicEngine.hpp"

class MassSpringBody2RigidBodyDynamicEngine : public DynamicEngine
{

	// construction
	public : MassSpringBody2RigidBodyDynamicEngine ();
	public : ~MassSpringBody2RigidBodyDynamicEngine ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	//public : void respondToInteractions(std::vector<shared_ptr<Body> >& bodies, const std::list<shared_ptr<Interaction> >& interactions,Real dt);
	public : void respondToInteractions(Body* body);
	REGISTER_CLASS_NAME(MassSpringBody2RigidBodyDynamicEngine);
};

REGISTER_SERIALIZABLE(MassSpringBody2RigidBodyDynamicEngine,false);

#endif // __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__
