 #include "Box2AABBFactory.hpp"
 #include "Box.hpp"
 #include "AABB.hpp"
  
Box2AABBFactory::Box2AABBFactory () : BoundingVolumeFactory()
{

}

Box2AABBFactory::~Box2AABBFactory ()
{

}

void Box2AABBFactory::processAttributes()
{

}

void Box2AABBFactory::registerAttributes()
{

}

shared_ptr<BoundingVolume> Box2AABBFactory::buildBoundingVolume(const shared_ptr<CollisionModel> cm, const Se3&)
{
	shared_ptr<Box> box = dynamic_pointer_cast<Box>(cm);
	
	return shared_ptr<BoundingVolume>(new AABB(box->extents,Vector3(0,0,0)));
}	
