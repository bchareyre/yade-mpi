#include"ParallelEngine.hpp"
#include<boost/python.hpp>
using namespace boost;
//#include<omp.h> // needed for omp_get_thread_num() (debugging)
YADE_PLUGIN((ParallelEngine));

//! ParallelEngine's pseudo-ctor (factory), taking nested lists of slave engines (might be moved to real ctor perhaps)
shared_ptr<ParallelEngine> ParallelEngine_ctor_list(const python::list& slaves){ shared_ptr<ParallelEngine> instance(new ParallelEngine); instance->slaves_set(slaves); return instance; }

void ParallelEngine::action(Scene*){
	// openMP warns if the iteration variable is unsigned...
	const int size=(int)slaves.size();
	#ifdef YADE_OPENMP
		#pragma omp parallel for
	#endif
	for(int i=0; i<size; i++){
		// run every slave group sequentially
		FOREACH(const shared_ptr<Engine>& e, slaves[i]) {
			//cerr<<"["<<omp_get_thread_num()<<":"<<e->getClassName()<<"]";
			e->scene=scene;
			if(e->isActivated(scene)) { e->action(scene); }
		}
	}
}

void ParallelEngine::slaves_set(const python::list& slaves2){
	int len=python::len(slaves2);
	slaves.clear();
	for(int i=0; i<len; i++){
		python::extract<std::vector<shared_ptr<Engine> > > serialGroup(slaves2[i]);
		if (serialGroup.check()){ slaves.push_back(serialGroup()); continue; }
		python::extract<shared_ptr<Engine> > serialAlone(slaves2[i]);
		if (serialAlone.check()){ vector<shared_ptr<Engine> > aloneWrap; aloneWrap.push_back(serialAlone()); slaves.push_back(aloneWrap); continue; }
		PyErr_SetString(PyExc_TypeError,"List elements must be either\n (a) sequences of engines to be executed one after another\n(b) alone engines.");
		python::throw_error_already_set();
	}
}

python::list ParallelEngine::slaves_get(){
	python::list ret;
	FOREACH(vector<shared_ptr<Engine > >& grp, slaves){
		if(grp.size()==1) ret.append(python::object(grp[0]));
		else ret.append(python::object(grp));
	}
	return ret;
}


