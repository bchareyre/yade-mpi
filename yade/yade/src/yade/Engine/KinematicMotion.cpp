#include "KinematicMotion.hpp"


KinematicMotion::KinematicMotion() : Actor()
{
	//startTime = -1;
	//stopTime = -1;
	time = 0;
	subscribedBodies.resize(0);
}

void KinematicMotion::registerAttributes()
{
	REGISTER_ATTRIBUTE(subscribedBodies);
}


void KinematicMotion::action(Body* b)
{
	this->moveToNextTimeStep(b);
}
