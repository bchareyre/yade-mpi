#include "BoundingSphere.hpp"

BoundingSphere::BoundingSphere (float r) : BoundingVolume()
{
	radius = r;
}


BoundingSphere::~BoundingSphere ()
{

}


void BoundingSphere::move(Se3r& )
{


}

void BoundingSphere::postProcessAttributes(bool)
{

}

void BoundingSphere::registerAttributes()
{
	BoundingVolume::registerAttributes();
	REGISTER_ATTRIBUTE(radius);
	REGISTER_ATTRIBUTE(center);
}

bool BoundingSphere::loadFromFile(char * )
{
	return false; 
}

void BoundingSphere::glDraw()
{	
	glColor3fv(color);
	glTranslatef(center[0],center[1],center[2]);
	glutWireSphere(radius,10,10);
}

void BoundingSphere::update(Se3r& se3)
{
	Vector3r v(radius,radius,radius);
	center = se3.translation;
	min = center-v;
	max = center+v;	
}
