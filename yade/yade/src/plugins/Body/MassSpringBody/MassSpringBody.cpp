#include "MassSpringBody.hpp"

// FIXME : redisign must not use a particular GM,CM or BV into Body
#include "Mesh2D.hpp"
#include "AABB.hpp"
#include "Constants.hpp"

MassSpringBody::MassSpringBody () : ConnexBody()
{
}


MassSpringBody::~MassSpringBody()
{

}

void MassSpringBody::processAttributes()
{
	ConnexBody::processAttributes();
	
	//FIXME : when serialization tracks pointers delete it
	//cm = dynamic_pointer_cast<CollisionModel>(gm);
	gm = cm;
	
}

void MassSpringBody::registerAttributes()
{
	ConnexBody::registerAttributes();
	REGISTER_ATTRIBUTE(stiffness);
	REGISTER_ATTRIBUTE(damping);
	REGISTER_ATTRIBUTE(properties);
	REGISTER_ATTRIBUTE(initialLengths);

}


void MassSpringBody::updateBoundingVolume(Se3& )
{
	Vector3 max = Vector3(-Constants::MAX_FLOAT,-Constants::MAX_FLOAT,-Constants::MAX_FLOAT);
	Vector3 min = Vector3(Constants::MAX_FLOAT,Constants::MAX_FLOAT,Constants::MAX_FLOAT);

	shared_ptr<Mesh2D> mesh = dynamic_pointer_cast<Mesh2D>(gm);
	vector<Vector3>& vertices = mesh->vertices;
	
	for(unsigned int i=0;i<vertices.size();i++)
	{
		max = max.maxTerm(vertices[i]);
		min = min.minTerm(vertices[i]);
	}

	// FIXME : dirty needs redesign
	shared_ptr<AABB> aabb = dynamic_pointer_cast<AABB>(bv);
	aabb->center = (max+min)*0.5;
	aabb->halfSize = (max-min)*0.5;
	aabb->min = aabb->center-aabb->halfSize;
	aabb->max = aabb->center+aabb->halfSize;

}

void MassSpringBody::updateCollisionModel(Se3& )
{
}
