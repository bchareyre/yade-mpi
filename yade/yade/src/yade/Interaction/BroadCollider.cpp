#include "BroadCollider.hpp"


BroadCollider::BroadCollider () : Actor()
{
}
	
BroadCollider::~BroadCollider ()
{

}


void BroadCollider::processAttributes()
{

}

void BroadCollider::registerAttributes()
{

}

void BroadCollider::action(Body* b)
{
	this->broadCollisionTest(b);
}
