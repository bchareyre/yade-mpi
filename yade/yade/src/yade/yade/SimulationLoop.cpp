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
	int turnId = Omega::instance().getNewTurnId();
	while (true) //FIXME : change that !!
	{
		
		Omega::instance().waitMyTurn(turnId); // now if only we could calculate without waiting for draw to finish drawing....
		if (Omega::instance().rootBody)
		{	
			Omega::instance().rootBody->moveToNextTimeStep();
			Omega::instance().incrementCurrentIteration();
			Omega::instance().incrementSimulationTime();
		}
		Omega::instance().endMyTurn();
	}
}
