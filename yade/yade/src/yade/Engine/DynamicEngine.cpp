#include "DynamicEngine.hpp"

DynamicEngine::DynamicEngine() : Actor()
{

}
	
DynamicEngine::~DynamicEngine ()
{

}

void DynamicEngine::postProcessAttributes(bool)
{

}

void DynamicEngine::registerAttributes()
{

}

void DynamicEngine::action(Body* b)
{
	this->respondToInteractions(b);
}
