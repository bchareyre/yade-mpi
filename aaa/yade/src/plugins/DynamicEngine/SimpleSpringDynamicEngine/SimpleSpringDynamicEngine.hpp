#ifndef __SIMPLESPRINGDYNAMICENGINE_H__
#define __SIMPLESPRINGDYNAMICENGINE_H__

#include "DynamicEngine.hpp"

class SimpleSpringDynamicEngine : public DynamicEngine
{
	private : std::vector<Vector3> prevVelocities;
	private : std::vector<Vector3> forces;
	private : std::vector<Vector3> couples;
	private : bool first;
	
	// construction
	public : SimpleSpringDynamicEngine ();
	public : ~SimpleSpringDynamicEngine ();
	
	public : void processAttributes();
	public : void registerAttributes();
	
	public : void respondToCollisions(std::vector<shared_ptr<Body> >& bodies, const std::list<shared_ptr<Contact> >& contacts,float dt);
	REGISTER_CLASS_NAME(SimpleSpringDynamicEngine);
};

REGISTER_CLASS(SimpleSpringDynamicEngine,false);

#endif // __SIMPLESPRINGDYNAMICENGINE_H__
