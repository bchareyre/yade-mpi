#include "CollisionGeometry.hpp"

CollisionGeometry::CollisionGeometry () : GeometricalModel(),Indexable()
{
}

CollisionGeometry::~CollisionGeometry ()
{

}

void CollisionGeometry::registerAttributes()
{
	GeometricalModel::registerAttributes();
}

void CollisionGeometry::processAttributes()
{
	GeometricalModel::processAttributes();
}
