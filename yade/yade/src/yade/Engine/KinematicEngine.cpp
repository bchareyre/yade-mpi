#include "KinematicEngine.hpp"


KinematicEngine::KinematicEngine() : Actor()
{
	//startTime = -1;
	//stopTime = -1;
	time = 0;
	subscribedBodies.resize(0);
}

KinematicEngine::~KinematicEngine() 
{

}



void KinematicEngine::afterDeserialization()
{

}

void KinematicEngine::registerAttributes()
{
	REGISTER_ATTRIBUTE(subscribedBodies);
}


void KinematicEngine::action(Body* b)
{
	this->moveToNextTimeStep(b);
}
