// 2008 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/StandAloneEngine.hpp>
#include<yade/core/MetaBody.hpp>
/* Execute a python command (in addPlotDataCall) periodically, with defined (and adjustable) periodicity.
 *
 * Period constraints are iterInterval and timeInterval. When either of them is exceeded, the addPlotDataCall is run.
 *
 * Thie engine is primarily conceived for collecting data for yade.plot plots during simulations, hence the name.
 */
class PlotDataGetter: public StandAloneEngine {
	private:
		long iterInterval;
		Real timeInterval;
		Real lastRecTime;
		long lastRecIter;
		string addPlotDataCall;
	public :
		PlotDataGetter(): iterInterval(0),timeInterval(0),lastRecTime(0),lastRecIter(0),addPlotDataCall("addPlotDataCall"){};
		virtual void action(MetaBody* b);
		virtual bool isActivated(){return true;}
		virtual void registerAttributes(){
			REGISTER_ATTRIBUTE(iterInterval);
			REGISTER_ATTRIBUTE(timeInterval);
			REGISTER_ATTRIBUTE(lastRecTime);
			REGISTER_ATTRIBUTE(lastRecIter);
			REGISTER_ATTRIBUTE(addPlotDataCall);
		}
	protected :
		virtual void postProcessAttributes(bool deserializing){}
	DECLARE_LOGGER;
	REGISTER_CLASS_NAME(PlotDataGetter);
	REGISTER_BASE_CLASS_NAME(StandAloneEngine);
};

REGISTER_SERIALIZABLE(PlotDataGetter,false);

