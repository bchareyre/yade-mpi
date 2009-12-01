// 2008 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/StandAloneEngine.hpp>
#include<yade/core/World.hpp>
#include<yade/pkg-common/PeriodicEngines.hpp>
/* Execute a python command (in addPlotDataCall) periodically, with defined (and adjustable) periodicity.
 *
 * Period constraints are iterInterval and timeInterval. When either of them is exceeded, the addPlotDataCall is run.
 *
 * Thie engine is primarily conceived for collecting data for yade.plot plots during simulations, hence the name.
 */
class PeriodicPythonRunner: public StretchPeriodicEngine {
	private:
		string command;
	public :
		PeriodicPythonRunner(): command("pass"){};
		/* virtual bool isActivated: not overridden, StretchPeriodicEngine handles that */
		virtual void action(World* b){
			PyGILState_STATE gstate;
				gstate = PyGILState_Ensure();
				PyRun_SimpleString(command.c_str()); // this is suboptimal, since it has to be parsed at every execution; critical?
			PyGILState_Release(gstate);
		}
	REGISTER_ATTRIBUTES(StretchPeriodicEngine,(command));
	REGISTER_CLASS_NAME(PeriodicPythonRunner);
	REGISTER_BASE_CLASS_NAME(StretchPeriodicEngine);
};

REGISTER_SERIALIZABLE(PeriodicPythonRunner);

