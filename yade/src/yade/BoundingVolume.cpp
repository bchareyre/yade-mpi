#include "BoundingVolume.hpp"

BoundingVolume::BoundingVolume () : Serializable()
{
}

BoundingVolume::~BoundingVolume ()
{

}

void BoundingVolume::registerAttributes()
{
	REGISTER_ATTRIBUTE(color);
}
