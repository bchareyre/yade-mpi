 #include "Box2AABBFactory.hpp"
 #include "Box.hpp"
 #include "AABB.hpp"
  
Box2AABBFactory::Box2AABBFactory () : BoundingVolumeFactory()
{

}

Box2AABBFactory::~Box2AABBFactory ()
{

}

void Box2AABBFactory::postProcessAttributes(bool)
{

}

void Box2AABBFactory::registerAttributes()
{

}

shared_ptr<BoundingVolume> Box2AABBFactory::buildBoundingVolume(const shared_ptr<CollisionGeometry> cm, const Se3r&)
{
	shared_ptr<Box> box = dynamic_pointer_cast<Box>(cm);
	
	return shared_ptr<BoundingVolume>(new AABB(box->extents,Vector3r(0,0,0)));
}	
