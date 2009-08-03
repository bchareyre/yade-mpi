// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#include "ConstitutiveLawDispatcher.hpp"
YADE_PLUGIN((ConstitutiveLawDispatcher));
void ConstitutiveLawDispatcher::action(MetaBody* rootBody){
	#ifdef YADE_OPENMP
		const long size=rootBody->transientInteractions->size();
		#pragma omp parallel for
		for(long i=0; i<size; i++){
			const shared_ptr<Interaction>& I=(*rootBody->transientInteractions)[i];
	#else
		FOREACH(shared_ptr<Interaction> I, *rootBody->transientInteractions){
	#endif
		if(I->isReal()){
			assert(I->interactionGeometry); assert(I->interactionPhysics);
			operator()(I->interactionGeometry,I->interactionPhysics,I.get(),rootBody);
		}
	}
}
