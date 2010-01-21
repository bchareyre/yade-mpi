#pragma once
#include<yade/core/GlobalEngine.hpp>
#include<boost/python.hpp>

class ParallelEngine;
shared_ptr<ParallelEngine> ParallelEngine_ctor_list(const python::list& slaves);

class ParallelEngine: public Engine {
	public:
		typedef vector<vector<shared_ptr<Engine> > > slaveContainer;
		slaveContainer slaves;
		ParallelEngine(){};
		virtual ~ParallelEngine(){};
		virtual void action(Scene*);
		virtual bool isActivated(Scene*){return true;}
	// py access
		boost::python::list slaves_get();
		void slaves_set(const boost::python::list& slaves);
	YADE_CLASS_BASE_ATTRS_PY(ParallelEngine,Engine,(slaves),
		.def("__init__",python::make_constructor(ParallelEngine_ctor_list))
		.add_property("slaves",&ParallelEngine::slaves_get,&ParallelEngine::slaves_set);
	);
};
REGISTER_SERIALIZABLE(ParallelEngine);


