#ifndef __CLOTH2RIGIDBODYNAMICENGINE_H__
#define __CLOTH2RIGIDBODYNAMICENGINE_H__

#include "DynamicEngine.hpp"

class Cloth2RigidBodyDynamicEngine : public DynamicEngine
{
	
	// construction
	public : Cloth2RigidBodyDynamicEngine ();
	public : ~Cloth2RigidBodyDynamicEngine ();
	
	public : void processAttributes();
	public : void registerAttributes();
	
	//public : void respondToCollisions(std::vector<shared_ptr<Body> >& bodies, const std::list<shared_ptr<Interaction> >& interactions,float dt);
	public : void respondToCollisions(Body* body, const std::list<shared_ptr<Interaction> >& interactions);
	REGISTER_CLASS_NAME(Cloth2RigidBodyDynamicEngine);
};

REGISTER_CLASS(Cloth2RigidBodyDynamicEngine,false);

#endif // __CLOTH2RIGIDBODYNAMICENGINE_H__
