// 2008 © Václav Šmilauer <eudoxos@arcig.cz>
#include"PlotDataGetter.hpp"
#include<Python.h>
YADE_PLUGIN("PlotDataGetter");
CREATE_LOGGER(PlotDataGetter);
void PlotDataGetter::action(MetaBody* rb){
	long iter=Omega::instance().getCurrentIteration();
	Real time=Omega::instance().getSimulationTime();
	if((iterInterval>0 && (iter-lastRecIter>=iterInterval)) || (timeInterval>0 && (time-lastRecTime>=timeInterval))){
		LOG_DEBUG("#"<<iter<<" "<<time<<"s (#"<<iter-lastRecIter<<" "<<time-lastRecTime<<" since last run), running `"<<addPlotDataCall<<"'");
		lastRecIter=iter; lastRecTime=time;
		// here, we use python API, but there already is an interpreter (MainLoop) running
		// if no GIL (GlobalInterpreterLock) → segfault!
		PyGILState_STATE gstate;
			gstate = PyGILState_Ensure();
			PyRun_SimpleString(addPlotDataCall.c_str()); // OK, this is suboptimal, since it has to be parsed at every execution; critical?
		PyGILState_Release(gstate);
	}
}
