#ifndef __SIMPLEBROADCOLLIDER_H__
#define __SIMPLEBROADCOLLIDER_H__

#include "BroadPhaseCollider.hpp"

class SimpleBroadCollider : public BroadPhaseCollider
{	
	// number of potential collision = number of colliding AABB
	protected : int nbPotentialCollisions;
	
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	// construction
	public : SimpleBroadCollider ();
	public : ~SimpleBroadCollider ();

	public : void processAttributes();
	public : void registerAttributes();

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	// return a list "contacts" of pairs of Body which Bounding volume are in collisions
	public : int broadPhaseCollisionTest(const std::vector<shared_ptr<Body> >& bodies,std::list<shared_ptr<Contact> >& contacts);
	
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

	REGISTER_CLASS_NAME(SimpleBroadCollider);

};

REGISTER_CLASS(SimpleBroadCollider,false);

#endif // __SIMPLEBROADCOLLIDER_H__ 


/*
	protected : static bool overlappSphereSphere(const shared_ptr<BoundingVolume> bv1, const shared_ptr<BoundingVolume> bv2);
	protected : static inline bool overlappSphereAABB(const shared_ptr<BoundingVolume> bv1, const shared_ptr<BoundingVolume> bv2)
	{
		return overlappAABBSphere(bv2,bv1);
	}
	protected : static bool overlappAABBSphere(const shared_ptr<BoundingVolume> bv1, const shared_ptr<BoundingVolume> bv2);
	protected : static bool overlappAABBAABB(const shared_ptr<BoundingVolume> bv1, const shared_ptr<BoundingVolume> bv2);
*/
