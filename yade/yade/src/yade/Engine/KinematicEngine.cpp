#include "KinematicEngine.hpp"


KinematicEngine::KinematicEngine() : Serializable()
{
	//startTime = -1;
	//stopTime = -1;
	time = 0;
	subscribedBodies.resize(0);
}

KinematicEngine::~KinematicEngine() 
{

}



void KinematicEngine::processAttributes()
{

}

void KinematicEngine::registerAttributes()
{
	REGISTER_ATTRIBUTE(subscribedBodies);
}
