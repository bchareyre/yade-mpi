#include "RigidBody.hpp"

RigidBody::RigidBody () : ConnexBody()
{
	acceleration = Vector3(0,0,0);
}


RigidBody::~RigidBody()
{

}

void RigidBody::processAttributes()
{
	ConnexBody::processAttributes();
	invMass = 1.0/mass;
	invInertia = Vector3(1/inertia[0],1/inertia[1],1/inertia[2]);
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

void RigidBody::moveToNextTimeStep()
{	
	ConnexBody::moveToNextTimeStep();
	prevAcceleration = acceleration;
	acceleration = Vector3(0,0,0);
	
}
