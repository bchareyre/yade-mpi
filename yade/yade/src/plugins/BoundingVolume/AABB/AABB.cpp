#include "AABB.hpp"

AABB::AABB () : BoundingVolume()
{

}

AABB::AABB (Vector3 hs,Vector3 c) : BoundingVolume()
{
	halfSize = hs;
	center = c;
	processAttributes();
}



AABB::~AABB ()
{

}

void AABB::processAttributes()
{
	initialHalfSize = halfSize;
	initialCenter = center;
	min = center-halfSize;
	max = center+halfSize;
}

void AABB::registerAttributes()
{	
	BoundingVolume::registerAttributes();
	REGISTER_ATTRIBUTE(center);
	REGISTER_ATTRIBUTE(halfSize);
}

void AABB::move(Se3& )
{


}

bool AABB::loadFromFile(char * )
{
	return false; 
}

void AABB::glDraw()
{
	glColor3fv(color);	
	glTranslatef(center[0],center[1],center[2]);
	glScalef(2*halfSize[0],2*halfSize[1],2*halfSize[2]);
	glDisable(GL_LIGHTING);
	glutWireCube(1);
}

void AABB::update(Se3& se3)
{
	//center = se3.translation;
	Vector3 newHalf,newCenter;	

 	center = se3.translation;// + se3.rotation * initialCenter;
	
	Matrix3 r;
	se3.rotation.toRotationMatrix(r);
	halfSize = Vector3(0,0,0);
	for( int i=0; i<3; ++i )
		for( int j=0; j<3; ++j )
			halfSize[i] += fabs( r[i][j] * initialHalfSize[j] );
	
	min = center-halfSize;
	max = center+halfSize;

}

bool AABB::overlap(const AABB& aabb)
{
	Vector3 max2 = aabb.max;
	Vector3 min2 = aabb.min;
	
	return !(max[0]<min2[0] || max2[0]<min[0] || max[1]<min2[1] || max2[1]<min[1] || max[2]<min2[2] || max2[2]<min[2]);               
}
