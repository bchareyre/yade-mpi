#ifndef __SIMPLESPRINGDYNAMICENGINE_H__
#define __SIMPLESPRINGDYNAMICENGINE_H__

#include "DynamicEngine.hpp"

class SimpleSpringDynamicEngine : public DynamicEngine
{
	private : std::vector<Vector3r> prevVelocities;
	private : std::vector<Vector3r> forces;
	private : std::vector<Vector3r> couples;
	private : bool first;

	// construction
	public : SimpleSpringDynamicEngine ();
	
	public : void registerAttributes();

	//public : void respondToInteractions(std::vector<shared_ptr<Body> >& bodies, const std::list<shared_ptr<Interaction> >& interactions,Real dt);
	public : void respondToInteractions(Body* body);
	REGISTER_CLASS_NAME(SimpleSpringDynamicEngine);
};

REGISTER_SERIALIZABLE(SimpleSpringDynamicEngine,false);

#endif // __SIMPLESPRINGDYNAMICENGINE_H__
