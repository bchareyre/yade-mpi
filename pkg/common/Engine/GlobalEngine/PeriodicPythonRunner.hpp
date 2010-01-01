// 2008 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/GlobalEngine.hpp>
#include<yade/core/Scene.hpp>
#include<yade/pkg-common/PeriodicEngines.hpp>
#include<yade/lib-pyutil/gil.hpp>
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
		virtual void action(Scene* b){ pyRunString(command); }
	REGISTER_ATTRIBUTES(StretchPeriodicEngine,(command));
	REGISTER_CLASS_NAME(PeriodicPythonRunner);
	REGISTER_BASE_CLASS_NAME(StretchPeriodicEngine);
};

REGISTER_SERIALIZABLE(PeriodicPythonRunner);

