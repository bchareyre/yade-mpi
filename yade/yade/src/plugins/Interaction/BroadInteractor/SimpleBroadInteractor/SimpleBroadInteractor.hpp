#ifndef __SIMPLEBROADCOLLIDER_H__
#define __SIMPLEBROADCOLLIDER_H__

#include "BroadInteractor.hpp"

class SimpleBroadInteractor : public BroadInteractor
{
	// number of potential interacions = number of interacting AABB
	protected : int nbPotentialInteractions;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

	// construction
	public : SimpleBroadInteractor ();
	public : ~SimpleBroadInteractor ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

	// return a list "interactions" of pairs of Body which Bounding volume are in potential interaction
	public : void broadInteractionTest(Body* body);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

	REGISTER_CLASS_NAME(SimpleBroadInteractor);

};

REGISTER_SERIALIZABLE(SimpleBroadInteractor,false);

#endif // __SIMPLEBROADCOLLIDER_H__


/*
	protected : static bool overlappSphereSphere(const shared_ptr<BodyBoundingVolume> bv1, const shared_ptr<BodyBoundingVolume> bv2);
	protected : static inline bool overlappSphereAABB(const shared_ptr<BodyBoundingVolume> bv1, const shared_ptr<BodyBoundingVolume> bv2)
	{
		return overlappAABBSphere(bv2,bv1);
	}
	protected : static bool overlappAABBSphere(const shared_ptr<BodyBoundingVolume> bv1, const shared_ptr<BodyBoundingVolume> bv2);
	protected : static bool overlappAABBAABB(const shared_ptr<BodyBoundingVolume> bv1, const shared_ptr<BodyBoundingVolume> bv2);
*/
