#include "SimulationLoop.hpp"
#include "NonConnexBody.hpp"
#include "Omega.hpp"
#include "ThreadSynchronizer.hpp"

SimulationLoop::SimulationLoop() : Threadable<SimulationLoop>(Omega::instance().synchronizer)
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
	if (Omega::instance().rootBody)
	{
		Omega::instance().rootBody->moveToNextTimeStep();
		Omega::instance().incrementCurrentIteration();
		Omega::instance().incrementSimulationTime();
	}
}
