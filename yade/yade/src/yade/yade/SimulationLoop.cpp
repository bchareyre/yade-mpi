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
		if (Omega::instance().rootBody)
		{
			Omega::instance().waitMyTurn(turnId); // now if only we could calculate without waiting for draw to finish drawing....
			Omega::instance().rootBody->moveToNextTimeStep();
			Omega::instance().incrementCurrentIteration();
			Omega::instance().endMyTurn();
		}
	}
}
