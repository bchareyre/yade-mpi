#include "SimulationLoop.hpp"
#include <yade/MetaBody.hpp>
#include <yade/Omega.hpp>
#include <yade-lib-threads/ThreadSynchronizer.hpp>

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
