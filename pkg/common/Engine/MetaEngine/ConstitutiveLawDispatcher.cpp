// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#include "ConstitutiveLawDispatcher.hpp"
YADE_PLUGIN("ConstitutiveLawDispatcher");

void ConstitutiveLawDispatcher::action(MetaBody* rootBody){
	FOREACH(shared_ptr<Interaction> I, *rootBody->transientInteractions){
		if(I->isReal){
			assert(I->interactionGeometry);
			assert(I->interactionPhysics);
			operator()(I->interactionGeometry,I->interactionPhysics,I.get(),rootBody);
		}
	}
}
