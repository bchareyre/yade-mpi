#ifndef __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__
#define __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__

#include "DynamicEngine.hpp"

class MassSpringBody2RigidBodyDynamicEngine : public DynamicEngine
{
	
	// construction
	public : MassSpringBody2RigidBodyDynamicEngine ();
	public : ~MassSpringBody2RigidBodyDynamicEngine ();
	
	public : void processAttributes();
	public : void registerAttributes();
	
	//public : void respondToCollisions(std::vector<shared_ptr<Body> >& bodies, const std::list<shared_ptr<Interaction> >& interactions,float dt);
	public : void respondToCollisions(Body* body, const std::list<shared_ptr<Interaction> >& interactions);
	REGISTER_CLASS_NAME(MassSpringBody2RigidBodyDynamicEngine);
};

REGISTER_CLASS(MassSpringBody2RigidBodyDynamicEngine,false);

#endif // __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__
