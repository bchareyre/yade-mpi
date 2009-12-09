// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#include "LawDispatcher.hpp"
YADE_PLUGIN((LawDispatcher));
void LawDispatcher::action(Scene*){
	updateScenePtr();
	#ifdef YADE_OPENMP
		const long size=scene->interactions->size();
		#pragma omp parallel for
		for(long i=0; i<size; i++){
			const shared_ptr<Interaction>& I=(*scene->interactions)[i];
	#else
		FOREACH(shared_ptr<Interaction> I, *scene->interactions){
	#endif
		if(I->isReal()){
			assert(I->interactionGeometry); assert(I->interactionPhysics);
			operator()(I->interactionGeometry,I->interactionPhysics,I.get(),scene);
		}
	}
}
