#pragma once
#include<yade/core/StandAloneEngine.hpp>
class ParallelEngine: public Engine {
	public:
		typedef vector<vector<shared_ptr<Engine> > > slaveContainer;
		slaveContainer slaves;
		ParallelEngine(){};
		virtual ~ParallelEngine(){};
		virtual void action(MetaBody*);
		virtual bool isActivated(){return true;}
		virtual list<string> getNeededBex();		
	protected:
		void registerAttributes(){Engine::registerAttributes(); REGISTER_ATTRIBUTE(slaves); }
	REGISTER_CLASS_NAME(ParallelEngine);
	REGISTER_BASE_CLASS_NAME(Engine);
};
REGISTER_SERIALIZABLE(ParallelEngine);
