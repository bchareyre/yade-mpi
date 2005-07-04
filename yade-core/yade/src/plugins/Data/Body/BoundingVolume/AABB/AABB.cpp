#include "AABB.hpp"
#include <yade-lib-opengl/OpenGLWrapper.hpp>

AABB::AABB () : BoundingVolume()
{
	createIndex();
}

AABB::~AABB ()
{
}
