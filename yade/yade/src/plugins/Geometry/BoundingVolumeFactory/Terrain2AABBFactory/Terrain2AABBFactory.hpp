
#ifndef __TERRAIN2AABBFACTORY_H__
#define __TERRAIN2AABBFACTORY_H__

#include "BoundingVolumeFactory.hpp"

class Terrain2AABBFactory : public BoundingVolumeFactory
{
	public : void go(	const shared_ptr<BodyInteractionGeometry>& cm,
				shared_ptr<BodyBoundingVolume>& bv,
				const Se3r& se3	);
};

REGISTER_FACTORABLE(Terrain2AABBFactory);

#endif // __TERRAIN2AABBFACTORY_H__
