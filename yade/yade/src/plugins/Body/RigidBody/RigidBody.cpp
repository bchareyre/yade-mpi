#include "RigidBody.hpp"

RigidBody::RigidBody () : ConnexBody()
{
	acceleration = Vector3(0,0,0);
	angularAcceleration = Vector3(0,0,0);
}


RigidBody::~RigidBody()
{

}

void RigidBody::processAttributes()
{
	ConnexBody::processAttributes();
	if (mass==0)
		invMass = 0;
	else
		invMass = 1.0/mass;
	
	for(int i=0;i<3;i++)
	{
		if (inertia[i]==0)
			invInertia[i] = 0;
		else
			invInertia[i] = 1.0/inertia[i];
	}
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

void RigidBody::updateCollisionGeometry(Se3& )
{

}

void RigidBody::moveToNextTimeStep()
{	
	ConnexBody::moveToNextTimeStep();
	prevAcceleration = acceleration;
	prevAngularAcceleration = angularAcceleration;
	acceleration = Vector3(0,0,0);
	angularAcceleration = Vector3(0,0,0);
	
}
