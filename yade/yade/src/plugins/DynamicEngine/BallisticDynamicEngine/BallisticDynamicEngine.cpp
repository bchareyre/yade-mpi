#include "BallisticDynamicEngine.hpp"
#include "RigidBody.hpp"
#include "Omega.hpp"
#include "NonConnexBody.hpp"

BallisticDynamicEngine::BallisticDynamicEngine () : DynamicEngine()
{
	first = true;
}

BallisticDynamicEngine::~BallisticDynamicEngine ()
{

}

void BallisticDynamicEngine::processAttributes()
{

}

void BallisticDynamicEngine::registerAttributes()
{
	DynamicEngine::registerAttributes();
	REGISTER_ATTRIBUTE(damping);
}


void BallisticDynamicEngine::respondToCollisions(Body * body, const std::list<shared_ptr<Interaction> >& interactions)
{
	RigidBody * rb = dynamic_cast<RigidBody*>(body);

	float dt = Omega::instance().dt;

	//rb->acceleration += Omega::instance().gravity;

	if (!first)
	{
		rb->velocity = damping*(prevVelocity+0.5*dt*rb->acceleration);
		rb->angularVelocity = damping*(prevAngularVelocity+0.5*dt*rb->angularAcceleration);
	}

	prevVelocity = rb->velocity+0.5*dt*rb->acceleration;
	prevAngularVelocity = rb->angularVelocity+0.5*dt*rb->angularAcceleration;


	rb->se3.translation += prevVelocity*dt;

	Vector3 axis = rb->angularVelocity;
	float angle = axis.unitize();
	Quaternion q;
	q.fromAngleAxis(angle*dt,axis);
	rb->se3.rotation = q*rb->se3.rotation;
	rb->se3.rotation.normalize();

	rb->updateBoundingVolume(rb->se3);

	first = false;

}

