#include "BallisticDynamicEngine.hpp"
#include "RigidBody.hpp"
#include "Omega.hpp"
#include "ComplexBody.hpp"

BallisticDynamicEngine::BallisticDynamicEngine () : DynamicEngine()
{
	first = true;
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
}


void BallisticDynamicEngine::respondToCollisions(Body * body)
{
	RigidBody * rb = dynamic_cast<RigidBody*>(body->physicalParameters.get());
	
	Real dt = Omega::instance().getTimeStep();

	if (!first)
	{
		rb->velocity = damping*(prevVelocity+0.5*dt*rb->acceleration);
		rb->angularVelocity = damping*(prevAngularVelocity+0.5*dt*rb->angularAcceleration);
	}

	prevVelocity = rb->velocity+0.5*dt*rb->acceleration;
	prevAngularVelocity = rb->angularVelocity+0.5*dt*rb->angularAcceleration;

	rb->se3.translation += prevVelocity*dt;

	Vector3r axis = rb->angularVelocity;
	Real angle = axis.normalize();
	Quaternionr q;
	q.fromAxisAngle(axis,angle*dt);
	rb->se3.rotation = q*rb->se3.rotation;
	rb->se3.rotation.normalize();

	first = false;
}

