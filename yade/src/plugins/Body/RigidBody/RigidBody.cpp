#include "RigidBody.hpp"

RigidBody::RigidBody () : ConnexBody()
{
}


RigidBody::~RigidBody()
{

}

void RigidBody::processAttributes()
{
	ConnexBody::processAttributes();
	invMass = 1.0;
	invInertia = Vector3(1,1,1);
}

void RigidBody::registerAttributes()
{
	ConnexBody::registerAttributes();
	REGISTER_ATTRIBUTE(inertia);
}


void RigidBody::updateBoundingVolume(Se3& se3)
{
	bv->update(se3);
}

void RigidBody::updateCollisionModel(Se3& )
{

}

void RigidBody::moveToNextTimeStep(float )
{
	//cout << mass << " " << isDynamic << endl;
}
