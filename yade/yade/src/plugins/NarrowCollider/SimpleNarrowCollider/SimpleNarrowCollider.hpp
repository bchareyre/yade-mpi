#ifndef __SIMPLENARROWCOLLIDER_H__
#define __SIMPLENARROWCOLLIDER_H__

#include "NarrowInteractor.hpp"

class SimpleNarrowCollider : public NarrowInteractor
{
	// construction
	public : SimpleNarrowCollider ();
	public : ~SimpleNarrowCollider ();

	public : void narrowCollisionPhase(Body* body);

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();
	REGISTER_CLASS_NAME(SimpleNarrowCollider);
};

REGISTER_SERIALIZABLE(SimpleNarrowCollider,false);

#endif // __SIMPLENARROWCOLLIDER_H__
