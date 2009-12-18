#pragma once
#include<yade/core/GlobalEngine.hpp>
#include<boost/python.hpp>
class ParallelEngine: public Engine {
	public:
		typedef vector<vector<shared_ptr<Engine> > > slaveContainer;
		slaveContainer slaves;
		ParallelEngine(){};
		virtual ~ParallelEngine(){};
		virtual void action(Scene*);
		virtual bool isActivated(Scene*){return true;}
		// python access
		boost::python::list slaves_get();
		void slaves_set(const boost::python::list& slaves);
	REGISTER_ATTRIBUTES(Engine,(slaves));
	REGISTER_CLASS_NAME(ParallelEngine);
	REGISTER_BASE_CLASS_NAME(Engine);
};
REGISTER_SERIALIZABLE(ParallelEngine);
