 #include "Terrain2AABBFactory.hpp"
 #include "Terrain.hpp"
 #include "AABB.hpp"
  
Terrain2AABBFactory::Terrain2AABBFactory () : BoundingVolumeFactory()
{
}

Terrain2AABBFactory::~Terrain2AABBFactory ()
{

}

void Terrain2AABBFactory::processAttributes()
{

}

void Terrain2AABBFactory::registerAttributes()
{

}

shared_ptr<BoundingVolume> Terrain2AABBFactory::buildBoundingVolume(const shared_ptr<CollisionGeometry> cm, const Se3& )
{
	shared_ptr<Terrain> t = dynamic_pointer_cast<Terrain>(cm);
	
	return shared_ptr<BoundingVolume>(new AABB((t->max-t->min)*0.5,(t->max+t->min)*0.5));
}	
