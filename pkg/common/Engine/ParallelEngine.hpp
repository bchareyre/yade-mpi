#pragma once
#include<yade/core/StandAloneEngine.hpp>
class ParallelEngine: public Engine {
	public:
		typedef vector<vector<shared_ptr<Engine> > > slaveContainer;
		slaveContainer slaves;
		ParallelEngine(){};
		virtual ~ParallelEngine(){};
		virtual void action(MetaBody*);
		virtual bool isActivated(MetaBody*){return true;}
		virtual list<string> getNeededBex();		
	REGISTER_ATTRIBUTES(Engine,(slaves));
	REGISTER_CLASS_NAME(ParallelEngine);
	REGISTER_BASE_CLASS_NAME(Engine);
};
REGISTER_SERIALIZABLE(ParallelEngine);
