
#ifndef __TERRAIN2AABBFACTORY_H__
#define __TERRAIN2AABBFACTORY_H__

#include "BoundingVolumeFactory.hpp"

class Terrain2AABBFactory : public BoundingVolumeFactory
{
	public : void go(	const shared_ptr<CollisionGeometry>& cm,
				shared_ptr<BoundingVolume>& bv,
				const Se3r& se3	);
};

REGISTER_FACTORABLE(Terrain2AABBFactory);

#endif // __TERRAIN2AABBFACTORY_H__
