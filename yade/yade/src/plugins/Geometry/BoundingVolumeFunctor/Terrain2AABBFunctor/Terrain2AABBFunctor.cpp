
#include "Terrain2AABBFunctor.hpp"
#include "Terrain.hpp"
#include "AABB.hpp"
  
void Terrain2AABBFunctor::go(	  const shared_ptr<InteractionDescription>&
				, const shared_ptr<BoundingVolume>&
				, const Se3r&
				, const Body* 	)
{
//	shared_ptr<Terrain> t = dynamic_pointer_cast<Terrain>(cm);
	
//	bv = shared_ptr<BoundingVolume>(new AABB((t->max-t->min)*0.5,(t->max+t->min)*0.5));
}	
