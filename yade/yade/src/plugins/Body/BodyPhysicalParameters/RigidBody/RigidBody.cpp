#include "RigidBody.hpp"

RigidBody::RigidBody () : Particle()
{
	createIndex();
	acceleration = Vector3r(0,0,0);
	angularAcceleration = Vector3r(0,0,0);
}


RigidBody::~RigidBody()
{

}

void RigidBody::postProcessAttributes(bool deserializing)
{
	Particle::postProcessAttributes(deserializing);
	
	if(deserializing)
	{
		
		for(int i=0;i<3;i++)
		{
			if (inertia[i]==0)
				invInertia[i] = 0;
			else
				invInertia[i] = 1.0/inertia[i];
		}
	}
}

void RigidBody::registerAttributes()
{
	Particle::registerAttributes();
	REGISTER_ATTRIBUTE(inertia);
	REGISTER_ATTRIBUTE(angularVelocity);
}

