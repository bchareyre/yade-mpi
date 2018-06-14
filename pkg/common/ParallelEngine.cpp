#include <pkg/common/ParallelEngine.hpp>
YADE_PLUGIN((ParallelEngine));

#ifdef YADE_OPENMP
  #include<omp.h>
#endif

//! ParallelEngine's pseudo-ctor (factory), taking nested lists of slave engines (might be moved to real ctor perhaps)
shared_ptr<ParallelEngine> ParallelEngine_ctor_list(const boost::python::list& slaves){ shared_ptr<ParallelEngine> instance(new ParallelEngine); instance->slaves_set(slaves); return instance; }

void ParallelEngine::action(){
	// openMP warns if the iteration variable is unsigned...
	const int size=(int)slaves.size();
	const bool TimingInfo_enabled=TimingInfo::enabled; 
	#ifdef YADE_OPENMP
		//nested parallel regions are disabled by default on some platforms, we enable them since some of the subengine may be also parallel
		omp_set_nested(1);
		#pragma omp parallel for num_threads(ompThreads)

	#endif
	for(int i=0; i<size; i++){
		TimingInfo::delta last=TimingInfo::getNow(); 
		// run every slave group sequentially
		FOREACH(const shared_ptr<Engine>& e, slaves[i]) {
			e->scene=scene;
			if(!e->dead && e->isActivated()) {
				e->action();
				if(TimingInfo_enabled) {TimingInfo::delta now=TimingInfo::getNow(); e->timingInfo.nsec+=now-last; e->timingInfo.nExec+=1; last=now;}
			}
		}
	}
}

void ParallelEngine::slaves_set(const boost::python::list& slaves2){
	int len=boost::python::len(slaves2);
	slaves.clear();
	for(int i=0; i<len; i++){
		boost::python::extract<std::vector<shared_ptr<Engine> > > serialGroup(slaves2[i]);
		if (serialGroup.check()){ slaves.push_back(serialGroup()); continue; }
		boost::python::extract<shared_ptr<Engine> > serialAlone(slaves2[i]);
		if (serialAlone.check()){ vector<shared_ptr<Engine> > aloneWrap; aloneWrap.push_back(serialAlone()); slaves.push_back(aloneWrap); continue; }
		PyErr_SetString(PyExc_TypeError,"List elements must be either\n (a) sequences of engines to be executed one after another\n(b) alone engines.");
		boost::python::throw_error_already_set();
	}
}

boost::python::list ParallelEngine::slaves_get(){
	boost::python::list ret;
	FOREACH(vector<shared_ptr<Engine > >& grp, slaves){
		if(grp.size()==1) ret.append(boost::python::object(grp[0]));
		else ret.append(boost::python::object(grp));
	}
	return ret;
}


