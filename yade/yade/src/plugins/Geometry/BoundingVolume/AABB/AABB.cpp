#include "AABB.hpp"
#include "OpenGLWrapper.hpp"

AABB::AABB () : BoundingVolume()
{
	createIndex();
}

AABB::~AABB ()
{
}
