#include "SimulationLoop.hpp"
#include "NonConnexBody.hpp"
#include "Omega.hpp"
#include "ThreadSynchronizer.hpp"

SimulationLoop::SimulationLoop()
{

}

SimulationLoop::~SimulationLoop()
{

}


void SimulationLoop::operator()()
{
	while (true)
	{
		if (Omega::instance().rootBody)
		{
			Omega::instance().synchronizer->wait1(); // now if only we could calculate without waiting for draw to finish drawing....
			Omega::instance().rootBody->moveToNextTimeStep();
			Omega::instance().synchronizer->go();
		}
	}
}
