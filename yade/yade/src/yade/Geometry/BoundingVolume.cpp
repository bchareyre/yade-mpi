#include "BoundingVolume.hpp"

BoundingVolume::BoundingVolume () : GeometricalModel(),Indexable()
{
}

BoundingVolume::~BoundingVolume ()
{

}

void BoundingVolume::registerAttributes()
{
	REGISTER_ATTRIBUTE(color);
}
