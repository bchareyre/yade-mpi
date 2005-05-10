
#include "Terrain2AABB.hpp"
#include <yade-common/Terrain.hpp>
#include <yade-common/AABB.hpp>
  
void Terrain2AABB::go(	  const shared_ptr<InteractingGeometry>&
				, shared_ptr<BoundingVolume>&
				, const Se3r&
				, const Body* 	)
{
//	shared_ptr<Terrain> t = dynamic_pointer_cast<Terrain>(cm);
	cerr << "Terrain2AABB: wtf ?\n";
//	bv = shared_ptr<BoundingVolume>(new AABB((t->max-t->min)*0.5,(t->max+t->min)*0.5));
}	
