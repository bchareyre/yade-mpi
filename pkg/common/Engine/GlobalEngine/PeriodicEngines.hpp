// 2008, 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#include<time.h>
#include<yade/core/GlobalEngine.hpp>
#include<yade/core/Omega.hpp>
/*!
	Run Engine::action with given fixed periodicity real time (=wall clock time, computation time),
	virtual time (simulation time), iteration number), by setting any of those criteria
	(virtPeriod, realPeriod, iterPeriod) to a positive value. They are all negative (inactive)
	by default.

	The number of times this engine is activated can be limited by setting nDo>0. If the number of activations
	will have been already reached, no action will be called even if an active period has elapsed.

	If initRun is set (false by default), the engine will run when called for the first time; otherwise it will only
	start counting period (realLast etc interal variables) from that point, but without actually running, and will run
	only once a period has elapsed since the initial run.

	This class should be used directly; rather, derive your own engine which you want to be run periodically.

	Derived engines should override Engine::action(Scene*), which will be called periodically. If the derived engine
	overrides also Engine::isActivated, it should also take in account return value from PeriodicEngine::isActivated,
	since otherwise the periodicity will not be functional.


	Example with PeriodicPythonRunner, which derives from PeriodicEngine; likely to be encountered in python scripts):

		PeriodicPythonRunner(realPeriod=5,iterPeriod=10000,command="print O.iter")

	will print iteration number every 10000 iterations or every 5 seconds of wall clock time, whiever comes first since it was
	last run.
 */
class PeriodicEngine:  public GlobalEngine {
	public:
		static Real getClock(){ timeval tp; gettimeofday(&tp,NULL); return tp.tv_sec+tp.tv_usec/1e6; }
		//! Periodicity criterion using virtual (simulation) time (deactivated if <= 0, which is the default)
		Real virtPeriod;
		Real virtLast; // track virtual time we were executed last
		//! Periodicity criterion using real (wall clock, computation, human) time (deactivated by default)
		Real realPeriod;
		Real realLast; // track real time we were executed last
		//! Periodicity criterion using step number (deactivated by default)
		long iterPeriod;
		long iterLast; // track step number of last execution
		//! Limit number of executions by this number (deactivated if negative, which is the default)
		long nDo;
		long nDone; // track number of executions (cummulative)
		//! Run the first time we are called as well (false by default)
		bool initRun;
		PeriodicEngine(): virtPeriod(0),virtLast(0),realPeriod(0),realLast(0),iterPeriod(0),iterLast(0),nDo(-1),nDone(0),initRun(false) { realLast=getClock(); }
		virtual bool isActivated(Scene*){
			Real virtNow=Omega::instance().getSimulationTime();
			Real realNow=getClock();
			long iterNow=Omega::instance().getCurrentIteration();
			if((nDo<0 || nDone<nDo) &&
				((virtPeriod>0 && virtNow-virtLast>=virtPeriod) ||
				 (realPeriod>0 && realNow-realLast>=realPeriod) ||
				 (iterPeriod>0 && iterNow-iterLast>=iterPeriod))){
				realLast=realNow; virtLast=virtNow; iterLast=iterNow; nDone++;
				return true;
			}
			if(nDone==0){
				realLast=realNow; virtLast=virtNow; iterLast=iterNow; nDone++;
				if(initRun) return true;
				return false;
			}
			return false;
		}
	REGISTER_ATTRIBUTES(GlobalEngine,
		(virtPeriod)
		(realPeriod)
		(iterPeriod)
		(virtLast)
		(realLast)
		(iterLast)
		(nDo)
		(nDone)
	)
	REGISTER_CLASS_AND_BASE(PeriodicEngine,GlobalEngine);
};
REGISTER_SERIALIZABLE(PeriodicEngine);

/*!
	PeriodicEngine but with constraint that may be stretched by a given stretchFactor (default 2).
	Limits for each periodicity criterion may be set and the mayStretch bool says whether the period
	can be stretched (default: doubled) without active criteria getting off limits.

	stretchFactor must be positive; if >1, period is stretched, for <1, it is shrunk.

	Limit consistency (whether actual period is not over/below the limit) is checked: period is set to the 
	limit value if we are off. If the limit is zero, however, and the period is non-zero, the limit is set
	to the period value (therefore, if you initialize only iterPeriod, you will get what you expect: engine
	running at iterPeriod).

	Note: the logic here is probably too complicated to be practical, although it works. Chances are that
	if unused, it will be removed from the codebase.
*/
class StretchPeriodicEngine: public PeriodicEngine{
	public:
	StretchPeriodicEngine(): PeriodicEngine(), realLim(0.), virtLim(0.), iterLim(0), stretchFactor(2.), mayStretch(false){}
	Real realLim, virtLim; long iterLim;
	Real stretchFactor;
	bool mayStretch;
	virtual bool isActivated(Scene* rootBody){
		assert(stretchFactor>0);
		if(iterLim==0 && iterPeriod!=0){iterLim=iterPeriod;} else if(iterLim!=0 && iterPeriod==0){iterPeriod=iterLim;}
		if(realLim==0 && realPeriod!=0){realLim=realPeriod;} else if(realLim!=0 && realPeriod==0){realPeriod=realLim;}
		if(virtLim==0 && virtPeriod!=0){virtLim=virtPeriod;} else if(virtLim!=0 && virtPeriod==0){virtPeriod=virtLim;}
		if(stretchFactor>1){iterPeriod=min(iterPeriod,iterLim); realPeriod=min(realPeriod,realLim); virtPeriod=min(virtPeriod,virtLim);}
		else {iterPeriod=max(iterPeriod,iterLim); realPeriod=max(realPeriod,realLim); virtPeriod=max(virtPeriod,virtLim);}
		mayStretch=((virtPeriod<0 || (stretchFactor>1 ? stretchFactor*virtPeriod<=virtLim : stretchFactor*virtPeriod>=virtLim))
		&& (realPeriod<0 || (stretchFactor>1 ? stretchFactor*realPeriod<=realLim : stretchFactor*realPeriod>=realLim))
		&& (iterPeriod<0 || (stretchFactor>1 ? stretchFactor*iterPeriod<=iterLim : stretchFactor*iterPeriod>=iterLim)));
		return PeriodicEngine::isActivated(rootBody);
	}
	REGISTER_ATTRIBUTES(PeriodicEngine,(realLim)(virtLim)(iterLim)(mayStretch)(stretchFactor));
	REGISTER_CLASS_NAME(StretchPeriodicEngine);
	REGISTER_BASE_CLASS_NAME(PeriodicEngine);
};
REGISTER_SERIALIZABLE(StretchPeriodicEngine);

