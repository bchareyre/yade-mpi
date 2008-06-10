// 2008 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#include<time.h>
#include<yade/core/StandAloneEngine.hpp>
#include<yade/core/Omega.hpp>

/* Run an action with adjustable and constrained periodicity (real time, virtual time, iteration)
 *
 * 3 criteria can be used: iteration period, realTime (wallclock) period, virtTime (simulation time) period.
 * Each of them is composed of a Vector3r with the meaning [lower limit, actual period value, upper limit].
 * The criterion is disabled if the lower limit is < 0, which is the default.
 * If _any_ criterion from the enabled ones is satisfied (elapsed iteration period etc.), the engine becomes active.
 *
 * At the first run after construction, the engine will ensure period consistency,
 * i.e. that on enabled periods, limits are properly ordered (swapped otherwise)
 * and that the actual value is not over the upper limit (will be made equal to it otherwise) ot under the lower limit.
 *
 * If it is useful to make the actual periods smaller/larger, mayDouble and mayHalve signify whether actual periods
 * (considering only enabled criteria) may be halved/doubled without getting off limits.
 *
 * This engine may be used only by deriving an engine with something useful in action(MetaBody*);
 * if used as-is, it will throw when activated.
 *
 * @bug we use gettimeofday() to get current time; benchmark it, since it involves a syscall?; better use HPET timer (async?) instead of polling the clock?
 */
class PeriodicEngine: public StandAloneEngine {
	private:
		Vector3r virtTimeLim,realTimeLim,iterLim;
		Real lastRealTime,lastVirtTime; long lastIter;
		bool mayDouble,mayHalve;
		bool perhapsInconsistent;
		void ensureConsistency(Vector3r& v){
			if(v[0]<0) return; // not active
			if(v[2]<v[0]){Real lo=v[2]; v[2]=v[0]; v[0]=lo;} // swap limits if necessary
			if(v[1]<v[0]) v[1]=v[0]; // put actual value below the lower limit to the lower limit
			if(v[1]>v[2]) v[2]=v[1]; // put actual value above the upper limit to the upper limit
		}
	public :
		PeriodicEngine(): virtTimeLim(-1,0,0),realTimeLim(-1,0,0),iterLim(-1,0,0), lastRealTime(0.),lastVirtTime(0.),lastIter(0),mayDouble(false),mayHalve(false),perhapsInconsistent(true){};
		virtual void action(MetaBody* b) { throw; }
		virtual bool isActivated(){
			if(perhapsInconsistent){ ensureConsistency(virtTimeLim); ensureConsistency(realTimeLim); ensureConsistency(iterLim); perhapsInconsistent=false; }

			mayDouble=((virtTimeLim[0]<0 || 2*virtTimeLim[1]<=virtTimeLim[2]) && (realTimeLim[0]<0 || 2*realTimeLim[1]<=realTimeLim[2]) && (iterLim[0]<0 || 2*iterLim[1]<=iterLim[2]));
			mayHalve=((virtTimeLim[0]<0 || .5*virtTimeLim[1]>=virtTimeLim[0]) && (realTimeLim[0]<0 && .5*realTimeLim[1]>=realTimeLim[0]) && (iterLim[0]<0 || .5*iterLim[1]>=iterLim[0]));
			
			long nowIter=Omega::instance().getCurrentIteration();
			Real nowVirtTime=Omega::instance().getSimulationTime();
			timeval tp; gettimeofday(&tp,NULL); Real nowRealTime=tp.tv_sec+tp.tv_usec/1e6;
			//cerr<<"--------------------"<<endl; cerr<<"virt:"<<virtTimeLim<<";"<<lastVirtTime<<";"<<nowVirtTime<<endl; cerr<<"real:"<<realTimeLim<<";"<<lastRealTime<<";"<<nowRealTime<<endl; cerr<<"iter:"<<iterLim<<";"<<lastIter<<";"<<nowIter<<endl;
			if (  (virtTimeLim[0]>=0 && nowVirtTime-lastVirtTime>virtTimeLim[1])
			 || (realTimeLim[0]>=0 && nowRealTime-lastRealTime>realTimeLim[1])
			 || (iterLim[0]>=0     && nowIter    -lastIter    >iterLim[1])){
				lastVirtTime=nowVirtTime; lastRealTime=nowRealTime; lastIter=nowIter;
				return true;
			} else return false;
		}
		virtual void registerAttributes(){
			StandAloneEngine::registerAttributes();
			REGISTER_ATTRIBUTE(virtTimeLim);
			REGISTER_ATTRIBUTE(realTimeLim);
			REGISTER_ATTRIBUTE(iterLim);
			REGISTER_ATTRIBUTE(lastRealTime);
			REGISTER_ATTRIBUTE(lastVirtTime);
			REGISTER_ATTRIBUTE(lastIter);
			REGISTER_ATTRIBUTE(mayDouble);
			REGISTER_ATTRIBUTE(mayHalve);
		}
	protected :
		virtual void postProcessAttributes(bool deserializing){}
	DECLARE_LOGGER;
	REGISTER_CLASS_NAME(PeriodicEngine);
	REGISTER_BASE_CLASS_NAME(StandAloneEngine);
};
REGISTER_SERIALIZABLE(PeriodicEngine,false);

