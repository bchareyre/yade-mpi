#ifndef __SIMPLENARROWCOLLIDER_H__
#define __SIMPLENARROWCOLLIDER_H__

#include "NarrowPhaseCollider.hpp"

class SimpleNarrowCollider : public NarrowPhaseCollider
{
	private : bool firstCall;
	
	// construction
	public : SimpleNarrowCollider ();
	public : ~SimpleNarrowCollider ();

	public : bool narrowCollisionPhase(const std::vector<shared_ptr<Body> >& bodies, std::list<shared_ptr<Contact> >& contacts);

	public : void buildNarrowCollisionManager(const std::vector<shared_ptr<Body> >& bodies);
	
	public : void processAttributes();
	public : void registerAttributes();
	REGISTER_CLASS_NAME(SimpleNarrowCollider);
};

REGISTER_CLASS(SimpleNarrowCollider,false);

#endif // __SIMPLENARROWCOLLIDER_H__
