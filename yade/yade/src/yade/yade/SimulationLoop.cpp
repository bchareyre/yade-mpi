#include "SimulationLoop.hpp"
#include "MetaBody.hpp"
#include "Omega.hpp"
#include "ThreadSynchronizer.hpp"

SimulationLoop::SimulationLoop() : Threadable<SimulationLoop>(Omega::instance().getSynchronizer())
{
	createThread();
}

SimulationLoop::~SimulationLoop()
{

}

bool SimulationLoop::notEnd()
{
	return true;
}

void SimulationLoop::oneLoop()
{
	LOCK(Omega::instance().getRootBodyMutex());
		
	if (Omega::instance().getRootBody())
	{
		Omega::instance().getRootBody()->moveToNextTimeStep();
		Omega::instance().incrementCurrentIteration();
		Omega::instance().incrementSimulationTime();
	}
}
