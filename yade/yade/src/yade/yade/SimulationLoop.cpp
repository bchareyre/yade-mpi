#include "SimulationLoop.hpp"
#include "NonConnexBody.hpp"

SimulationLoop::SimulationLoop() : thread(SimulationLoop::loop) 
{ 

//nt xtime_get(xtime* xtp, int clock_type);
//	sleep();
}

SimulationLoop::~SimulationLoop()
{

}
	
	
void SimulationLoop::loop()
{
	while (true)
	{
		if (Omega::instance().rootBody)
		{
			Omega::instance().rootBody->moveToNextTimeStep();
			Omega::instance().endOfSimulationLoop();
		}
	}

//		static bool progress = Omega::instance().getProgress();
//		static long int max  = Omega::instance().getMaxiter();
	
// 		if( frame % 100 == 0 )					// checks every 100th iteration
// 		{
// 			if(progress)
// 				cout << "iteration: " << frame << endl;
// 			if( max != 0 )
// 				if( frame > max )
// 				{
// 					cerr << "Calc finished at: " << frame << endl;
// 					exit(0);			// terminate.
// 				}
// 		}
	
}
