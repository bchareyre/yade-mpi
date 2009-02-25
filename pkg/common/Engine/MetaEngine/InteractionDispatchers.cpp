#include"InteractionDispatchers.hpp"

YADE_PLUGIN("InteractionDispatchers");

InteractionDispatchers::InteractionDispatchers(){
	geomDispatcher=shared_ptr<InteractionGeometryMetaEngine>(new InteractionGeometryMetaEngine);
	physDispatcher=shared_ptr<InteractionPhysicsMetaEngine>(new InteractionPhysicsMetaEngine);
	constLawDispatcher=shared_ptr<ConstitutiveLawDispatcher>(new ConstitutiveLawDispatcher);
}

void InteractionDispatchers::action(MetaBody* rootBody){
	#ifdef YADE_OPENMP
		const long size=rootBody->interactions->size();
		#pragma omp parallel for
		for(long i=0; i<size; i++){
			const shared_ptr<Interaction>& I=(*rootBody->interactions)[i];
	#else
		FOREACH(shared_ptr<Interaction> I, *rootBody->interactions){
	#endif
			// InteractionGeometryMetaEngine
			const shared_ptr<Body>& b1=Body::byId(I->getId1(),rootBody);
			const shared_ptr<Body>& b2=Body::byId(I->getId2(),rootBody);
			I->isReal =
				b1->interactingGeometry && b2->interactingGeometry && // some bodies do not have interactingGeometry
				geomDispatcher->operator()(b1->interactingGeometry, b2->interactingGeometry, b1->physicalParameters->se3, b2->physicalParameters->se3,I);
			if(!I->isReal) continue;
			// InteractionPhysicsMetaEngine
			physDispatcher->operator()(b1->physicalParameters, b2->physicalParameters,I);
			// ConstitutiveLawDispatcher
			constLawDispatcher->operator()(I->interactionGeometry,I->interactionPhysics,I.get(),rootBody);
		}
}
