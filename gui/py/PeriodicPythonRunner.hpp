// 2008 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/StandAloneEngine.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/PeriodicEngines.hpp>
/* Execute a python command (in addPlotDataCall) periodically, with defined (and adjustable) periodicity.
 *
 * Period constraints are iterInterval and timeInterval. When either of them is exceeded, the addPlotDataCall is run.
 *
 * Thie engine is primarily conceived for collecting data for yade.plot plots during simulations, hence the name.
 */
class PeriodicPythonRunner: public RangePeriodicEngine {
	private:
		string command;
	public :
		PeriodicPythonRunner(): command("pass"){};
		/* virtual bool isActivated: not overridden, PeriodicEngine handles that */
		virtual void action(MetaBody* b){
			//cerr<<"[PeriodicPythonRunner]";
			PyGILState_STATE gstate;
				gstate = PyGILState_Ensure();
				PyRun_SimpleString(command.c_str()); // this is suboptimal, since it has to be parsed at every execution; critical?
			PyGILState_Release(gstate);
		}
		virtual void registerAttributes(){ RangePeriodicEngine::registerAttributes(); REGISTER_ATTRIBUTE(command); }
	protected :
		virtual void postProcessAttributes(bool deserializing){}
	REGISTER_CLASS_NAME(PeriodicPythonRunner);
	REGISTER_BASE_CLASS_NAME(RangePeriodicEngine);
};

REGISTER_SERIALIZABLE(PeriodicPythonRunner,false);

