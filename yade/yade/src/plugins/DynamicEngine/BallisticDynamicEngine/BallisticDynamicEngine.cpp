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
}


void BallisticDynamicEngine::respondToCollisions(Body * body, const std::list<shared_ptr<Interaction> >& interactions)
{	
	RigidBody * rb = dynamic_cast<RigidBody*>(body);
	
	float dt = Omega::instance().dt;

	rb->acceleration += Omega::instance().gravity;
		
	if (!first)
		rb->velocity = 0.99*(prevVelocity+0.5*dt*rb->acceleration);

	prevVelocity = rb->velocity+0.5*dt*rb->acceleration;
	rb->se3.translation += prevVelocity*dt;

	rb->updateBoundingVolume(rb->se3);
	
	first = false;

}

