#include "BoundingSphere.hpp"
#include "OpenGLWrapper.hpp"

BoundingSphere::BoundingSphere () : BoundingVolume()
{
	createIndex();
}

BoundingSphere::~BoundingSphere ()
{
}

void BoundingSphere::registerAttributes() // FIXME - remove that function, dispatcher shoul take care of that
{
	BoundingVolume::registerAttributes();
	REGISTER_ATTRIBUTE(radius);
	REGISTER_ATTRIBUTE(center);
}

// void BoundingSphere::update(Se3r& se3)
// {
// 	Vector3r v(radius,radius,radius);
// 	center = se3.translation;
// 	min = center-v;
// 	max = center+v;	
// }
