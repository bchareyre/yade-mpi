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
			Omega::instance().synchronizer->wait1();
			Omega::instance().rootBody->moveToNextTimeStep();
			Omega::instance().synchronizer->go();
		}
	}
}
