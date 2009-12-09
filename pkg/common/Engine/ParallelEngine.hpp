#pragma once
#include<yade/core/GlobalEngine.hpp>
class ParallelEngine: public Engine {
	public:
		typedef vector<vector<shared_ptr<Engine> > > slaveContainer;
		slaveContainer slaves;
		ParallelEngine(){};
		virtual ~ParallelEngine(){};
		virtual void action(Scene*);
		virtual bool isActivated(Scene*){return true;}
	REGISTER_ATTRIBUTES(Engine,(slaves));
	REGISTER_CLASS_NAME(ParallelEngine);
	REGISTER_BASE_CLASS_NAME(Engine);
};
REGISTER_SERIALIZABLE(ParallelEngine);
