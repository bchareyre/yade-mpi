#include "SimulationLoop.hpp"
#include "NonConnexBody.hpp"
#include "Omega.hpp"
#include "ThreadSynchronizer.hpp"

SimulationLoop::SimulationLoop(shared_ptr<ThreadSynchronizer> s)
{
	createThread(s);
}

SimulationLoop::~SimulationLoop()
{

}

bool SimulationLoop::notEnd()
{
	return true;
}

//#define THREAD_DEBUG
	
void SimulationLoop::oneLoop()
{
#ifndef THREAD_DEBUG
	if (Omega::instance().rootBody)
	{	
		Omega::instance().rootBody->moveToNextTimeStep();
		Omega::instance().incrementCurrentIteration();
		Omega::instance().incrementSimulationTime();
	}
#else	
	ThreadSafe::cerr("mark:  1 " + string(typeid(*this).name()) );
	if (Omega::instance().rootBody)
	{	
		ThreadSafe::cerr("mark:  2 " + string(typeid(*this).name()) );
		
		Omega::instance().rootBody->moveToNextTimeStep();
		
		ThreadSafe::cerr("mark:  3 " + string(typeid(*this).name()) );
		Omega::instance().incrementCurrentIteration();
		
		ThreadSafe::cerr("mark:  4 " + string(typeid(*this).name()) );
		Omega::instance().incrementSimulationTime();
		
		ThreadSafe::cerr("mark:  5 " + string(typeid(*this).name()) );
	}
	ThreadSafe::cerr("mark:  6 " + string(typeid(*this).name()) );
#endif
}
