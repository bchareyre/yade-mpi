#include "CollisionGeometry.hpp"

CollisionGeometry::CollisionGeometry () : GeometricalModel(),Indexable()
{
}

CollisionGeometry::~CollisionGeometry ()
{

}

void CollisionGeometry::postProcessAttributes(bool deserializing)
{
	GeometricalModel::postProcessAttributes(deserializing);
}

void CollisionGeometry::registerAttributes()
{
	GeometricalModel::registerAttributes();
}
