#include "BroadCollider.hpp"


BroadCollider::BroadCollider () : Actor()
{
}
	
BroadCollider::~BroadCollider ()
{

}


void BroadCollider::postProcessAttributes(bool)
{

}

void BroadCollider::registerAttributes()
{

}

void BroadCollider::action(Body* b)
{
	this->broadCollisionTest(b);
}
