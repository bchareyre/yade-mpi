#ifndef __TERRAIN2AABBFACTORY_H__
#define __TERRAIN2AABBFACTORY_H__

#include "BoundingVolumeEngineUnit.hpp"

class Terrain2AABB : public BoundingVolumeEngineUnit
{
	public : void go(	  const shared_ptr<InteractingGeometry>&
				, shared_ptr<BoundingVolume>&
				, const Se3r&
				, const Body*	);
	REGISTER_CLASS_NAME(Terrain2AABB);
};

REGISTER_SERIALIZABLE(Terrain2AABB,false);

#endif // __TERRAIN2AABBFACTORY_H__
