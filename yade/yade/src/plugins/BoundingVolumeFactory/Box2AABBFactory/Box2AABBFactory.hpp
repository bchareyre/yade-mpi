
#ifndef __BOX2AABBFACTORY_H__
#define __BOX2AABBFACTORY_H__

#include "BoundingVolumeFactory.hpp"

class Box2AABBFactory : public BoundingVolumeFactory
{
	// construction
	public : Box2AABBFactory ();
	public : ~Box2AABBFactory ();

	public : void processAttributes();
	public : void registerAttributes();

	public : shared_ptr<BoundingVolume> buildBoundingVolume(const shared_ptr<CollisionGeometry> cm, const Se3& se3);
};

REGISTER_FACTORABLE(Box2AABBFactory);

#endif // __BOX2AABBFACTORY_H__
