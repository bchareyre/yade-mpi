#include "BroadInteractor.hpp"


BroadInteractor::BroadInteractor () : Actor()
{
}
	
BroadInteractor::~BroadInteractor ()
{

}


void BroadInteractor::postProcessAttributes(bool)
{

}

void BroadInteractor::registerAttributes()
{

}

void BroadInteractor::action(Body* b)
{
	this->broadInteractionTest(b);
}
