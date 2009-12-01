#include"ParallelEngine.hpp"
//#include<omp.h> // needed for omp_get_thread_num() (debugging)
YADE_PLUGIN((ParallelEngine));

void ParallelEngine::action(World* rootBody){
	// openMP warns if the iteration variable is unsigned...
	const int size=(int)slaves.size();
	#ifdef YADE_OPENMP
		#pragma omp parallel for
	#endif
	for(int i=0; i<size; i++){
		// run every slave group sequentially
		FOREACH(const shared_ptr<Engine>& e, slaves[i]) {
			//cerr<<"["<<omp_get_thread_num()<<":"<<e->getClassName()<<"]";
			e->world=world;
			if(e->isActivated(rootBody)) { e->action(rootBody); }
		}
	}
}
