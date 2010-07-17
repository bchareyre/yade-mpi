// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#include "LawDispatcher.hpp"
YADE_PLUGIN((LawDispatcher));
CREATE_LOGGER(LawDispatcher);
void LawDispatcher::action(){
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
			operator()(I->interactionGeometry,I->interactionPhysics,I.get());
			if(!I->isReal() && I->isFresh(scene)) LOG_ERROR("Law functor deleted interaction that was just created. Please report bug: either this message is spurious, or the functor (or something else) is buggy.");
		}
	}
}
