#include "MassSpringBody.hpp"

// FIXME : redisign must not use a particular GM,CM or BV into Body
#include "Mesh2D.hpp"
#include "AABB.hpp"
#include "Math.hpp"

MassSpringBody::MassSpringBody () : ConnexBody()
{
}


MassSpringBody::~MassSpringBody()
{

}

void MassSpringBody::postProcessAttributes(bool deserializing)
{
	ConnexBody::postProcessAttributes(deserializing);
	
	if(deserializing)
	{
		//FIXME : when serialization tracks pointers delete it
		//cm = dynamic_pointer_cast<CollisionGeometry>(gm);
		gm = cm;
	}
	
}

void MassSpringBody::registerAttributes()
{
	ConnexBody::registerAttributes();
	REGISTER_ATTRIBUTE(stiffness);
	REGISTER_ATTRIBUTE(damping);
	REGISTER_ATTRIBUTE(properties);
	REGISTER_ATTRIBUTE(initialLengths);

}


// void MassSpringBody::updateBoundingVolume(Se3r& )
// {
// 	Vector3r max = Vector3r(-Mathr::MAX_REAL,-Mathr::MAX_REAL,-Mathr::MAX_REAL);
// 	Vector3r min = Vector3r(Mathr::MAX_REAL,Mathr::MAX_REAL,Mathr::MAX_REAL);
// 
// 	shared_ptr<Mesh2D> mesh = dynamic_pointer_cast<Mesh2D>(gm);
// 	vector<Vector3r>& vertices = mesh->vertices;
// 	
// 	for(unsigned int i=0;i<vertices.size();i++)
// 	{
// 		max = max.maxVector(vertices[i]);
// 		min = min.minVector(vertices[i]);
// 	}
// 
// 	// FIXME : dirty needs redesign
// 	shared_ptr<AABB> aabb = dynamic_pointer_cast<AABB>(bv);
// 	aabb->center = (max+min)*0.5;
// 	aabb->halfSize = (max-min)*0.5;
// 	aabb->min = aabb->center-aabb->halfSize;
// 	aabb->max = aabb->center+aabb->halfSize;
// 
// }

