#ifndef __TERRAIN2AABBFACTORY_H__
#define __TERRAIN2AABBFACTORY_H__

#include "BoundingVolumeFunctor.hpp"

class Terrain2AABBFunctor : public BoundingVolumeFunctor
{
	public : void go(	  const shared_ptr<InteractionDescription>&
				, shared_ptr<BoundingVolume>&
				, const Se3r&
				, const Body*	);
	REGISTER_CLASS_NAME(Terrain2AABBFunctor);
};

REGISTER_SERIALIZABLE(Terrain2AABBFunctor,false);

#endif // __TERRAIN2AABBFACTORY_H__
