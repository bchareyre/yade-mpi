#ifndef __SIMPLENARROWCOLLIDER_H__
#define __SIMPLENARROWCOLLIDER_H__

#include "NarrowPhaseCollider.hpp"

class SimpleNarrowCollider : public NarrowPhaseCollider
{
	// construction
	public : SimpleNarrowCollider ();
	public : ~SimpleNarrowCollider ();

	public : bool narrowCollisionPhase(const std::vector<shared_ptr<Body> >& bodies, std::list<shared_ptr<Interaction> >& interactions);
	
	public : void processAttributes();
	public : void registerAttributes();
	REGISTER_CLASS_NAME(SimpleNarrowCollider);
};

REGISTER_CLASS(SimpleNarrowCollider,false);

#endif // __SIMPLENARROWCOLLIDER_H__
