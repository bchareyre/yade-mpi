#ifndef __SIMPLEBROADCOLLIDER_H__
#define __SIMPLEBROADCOLLIDER_H__

#include "BroadInteractor.hpp"

class BroadInteractionEngineUnit : public BroadInteractor
{
	// number of potential interacions = number of interacting AABB
	protected : int nbPotentialInteractions;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

	// construction
	public : BroadInteractionEngineUnit ();
	public : ~BroadInteractionEngineUnit ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

	// return a list "volatileInteractions" of pairs of Body which Bounding volume are in potential interaction
	public : void broadInteractionTest(Body* body);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

	REGISTER_CLASS_NAME(BroadInteractionEngineUnit);

};

REGISTER_SERIALIZABLE(BroadInteractionEngineUnit,false);

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
