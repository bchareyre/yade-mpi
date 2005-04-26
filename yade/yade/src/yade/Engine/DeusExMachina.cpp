#include "DeusExMachina.hpp"


DeusExMachina::DeusExMachina() : Engine()
{
	//startTime = -1;
	//stopTime = -1;
	time = 0;
	subscribedBodies.resize(0);
}

void DeusExMachina::registerAttributes()
{
	REGISTER_ATTRIBUTE(subscribedBodies);
}


void DeusExMachina::action(Body* b)
{
	this->applyCondition(b);
}
