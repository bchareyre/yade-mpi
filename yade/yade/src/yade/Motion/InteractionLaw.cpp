#include "DynamicEngine.hpp"

void DynamicEngine::action(Body* b)
{
	this->respondToInteractions(b);
}
