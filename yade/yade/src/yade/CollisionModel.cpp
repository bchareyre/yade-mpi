#include "CollisionModel.hpp"

CollisionModel::CollisionModel () : GeometricalModel(),Indexable()
{
}

CollisionModel::~CollisionModel ()
{

}

void CollisionModel::registerAttributes()
{
	GeometricalModel::registerAttributes();
}

void CollisionModel::processAttributes()
{
	GeometricalModel::processAttributes();
}
