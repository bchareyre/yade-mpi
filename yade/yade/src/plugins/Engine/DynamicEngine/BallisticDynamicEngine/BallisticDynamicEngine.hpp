#ifndef __BALLISTICDYNAMICENGINE_H__
#define __BALLISTICDYNAMICENGINE_H__

#include "DynamicEngine.hpp"

class BallisticDynamicEngine : public DynamicEngine
{
	private : Vector3r prevVelocity;
	private : Vector3r prevAngularVelocity;
	private : bool first;
	public : Real damping;

	// construction
	public : BallisticDynamicEngine ();
	public : ~BallisticDynamicEngine ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : void respondToCollisions(Body* body);
	REGISTER_CLASS_NAME(BallisticDynamicEngine);
};

REGISTER_SERIALIZABLE(BallisticDynamicEngine,false);

#endif // __BALLISTICDYNAMICENGINE_H__
