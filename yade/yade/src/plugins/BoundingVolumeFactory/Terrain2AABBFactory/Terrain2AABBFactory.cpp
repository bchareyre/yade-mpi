 #include "Terrain2AABBFactory.hpp"
 #include "Terrain.hpp"
 #include "AABB.hpp"
  
shared_ptr<BoundingVolume> Terrain2AABBFactory::go(	const shared_ptr<CollisionGeometry>& cm,
							shared_ptr<BoundingVolume>& bv,
							const Se3r& se3	)
{
	shared_ptr<Terrain> t = dynamic_pointer_cast<Terrain>(cm);
	
	return shared_ptr<BoundingVolume>(new AABB((t->max-t->min)*0.5,(t->max+t->min)*0.5));
}	
