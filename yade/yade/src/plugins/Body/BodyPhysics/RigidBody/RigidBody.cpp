#include "RigidBody.hpp"

RigidBody::RigidBody () : SimpleBody()
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
	SimpleBody::postProcessAttributes(deserializing);
	
	if(deserializing)
	{
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
}

void RigidBody::registerAttributes()
{
	SimpleBody::registerAttributes();
	REGISTER_ATTRIBUTE(inertia);
}


void RigidBody::moveToNextTimeStep()
{	
	//FIXME : move reseting to another place
	SimpleBody::moveToNextTimeStep();
	prevAcceleration = acceleration;
	prevAngularAcceleration = angularAcceleration;
	acceleration = Vector3r(0,0,0);
	angularAcceleration = Vector3r(0,0,0);
	
}
