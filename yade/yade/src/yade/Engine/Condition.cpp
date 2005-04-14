#include "Condition.hpp"


Condition::Condition() : Actor()
{
	//startTime = -1;
	//stopTime = -1;
	time = 0;
	subscribedBodies.resize(0);
}

void Condition::registerAttributes()
{
	REGISTER_ATTRIBUTE(subscribedBodies);
}


void Condition::action(Body* b)
{
	this->applyCondition(b);
}
