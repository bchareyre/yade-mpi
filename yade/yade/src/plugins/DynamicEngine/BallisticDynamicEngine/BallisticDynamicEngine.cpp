#include "BallisticDynamicEngine.hpp"
#include "RigidBody.hpp"
#include "Omega.hpp"
#include "NonConnexBody.hpp"

BallisticDynamicEngine::BallisticDynamicEngine () : DynamicEngine()
{
	first = true;
	recordForces = false;
}

BallisticDynamicEngine::~BallisticDynamicEngine ()
{

}

void BallisticDynamicEngine::postProcessAttributes(bool)
{

}

void BallisticDynamicEngine::registerAttributes()
{
	DynamicEngine::registerAttributes();
	REGISTER_ATTRIBUTE(damping);
	REGISTER_ATTRIBUTE(recordForces);
}


void BallisticDynamicEngine::respondToCollisions(Body * body)
{
	RigidBody * rb = dynamic_cast<RigidBody*>(body);
	
	if(recordForces)  // FIXME - hack for recording forces
	{
		cout << rb->acceleration[0] << " " << rb->acceleration[1] << " " << rb->acceleration[2] << endl;
		prevVelocity 		= Vector3r(0,0,0);
		prevAngularVelocity 	= Vector3r(0,0,0);
		rb->acceleration 	= Vector3r(0,0,0);
		rb->angularAcceleration = Vector3r(0,0,0);
		
	}
	else
	{
	
		float dt = Omega::instance().dt;
	
		//rb->acceleration += Omega::instance().getGravity();
	
		if (!first)
		{
			rb->velocity = damping*(prevVelocity+0.5*dt*rb->acceleration);
			rb->angularVelocity = damping*(prevAngularVelocity+0.5*dt*rb->angularAcceleration);
		}
	
		prevVelocity = rb->velocity+0.5*dt*rb->acceleration;
		prevAngularVelocity = rb->angularVelocity+0.5*dt*rb->angularAcceleration;
	
	
		rb->se3.translation += prevVelocity*dt;
	
		Vector3r axis = rb->angularVelocity;
		float angle = axis.normalize();
		Quaternionr q;
		q.fromAxisAngle(axis,angle*dt);
		rb->se3.rotation = q*rb->se3.rotation;
		rb->se3.rotation.normalize();
	
	//	rb->updateBoundingVolume(rb->se3);
	
		first = false;
	}
}

