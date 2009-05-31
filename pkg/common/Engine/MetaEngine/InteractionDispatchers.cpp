#include"InteractionDispatchers.hpp"

YADE_PLUGIN("InteractionDispatchers");
CREATE_LOGGER(InteractionDispatchers);

InteractionDispatchers::InteractionDispatchers(){
	geomDispatcher=shared_ptr<InteractionGeometryMetaEngine>(new InteractionGeometryMetaEngine);
	physDispatcher=shared_ptr<InteractionPhysicsMetaEngine>(new InteractionPhysicsMetaEngine);
	constLawDispatcher=shared_ptr<ConstitutiveLawDispatcher>(new ConstitutiveLawDispatcher);
	alreadyWarnedNoCollider=false;
}

#define DISPATCH_CACHE

void InteractionDispatchers::action(MetaBody* rootBody){
	if(rootBody->interactions->pendingErase.size()>0){
		if(!alreadyWarnedNoCollider){
			LOG_WARN("Interactions pending erase found, no collider being used?");
			alreadyWarnedNoCollider=true;
		}
		rootBody->interactions->unconditionalErasePending();
	}
	#ifdef YADE_OPENMP
		const long size=rootBody->interactions->size();
		#pragma omp parallel for
		for(long i=0; i<size; i++){
			const shared_ptr<Interaction>& I=(*rootBody->interactions)[i];
	#else
		FOREACH(shared_ptr<Interaction> I, *rootBody->interactions){
	#endif
		#ifdef DISPATCH_CACHE
			const shared_ptr<Body>& b1_=Body::byId(I->getId1(),rootBody);
			const shared_ptr<Body>& b2_=Body::byId(I->getId2(),rootBody);

			// we know there is no geometry functor already, take the short path
			if(!I->functorCache.geomExists) { assert(!I->isReal()); continue; }
			// no interaction geometry for either of bodies; no interaction possible
			if(!b1_->interactingGeometry || !b2_->interactingGeometry) { assert(!I->isReal()); continue; }

			bool swap=false;
			// InteractionGeometryMetaEngine
			if(!I->functorCache.geom || !I->functorCache.phys){
				I->functorCache.geom=geomDispatcher->getFunctor2D(b1_->interactingGeometry,b2_->interactingGeometry,swap);
				// returns NULL ptr if no functor exists; remember that and shortcut
				if(!I->functorCache.geom) { I->functorCache.geomExists=false; continue; }
			}
			// arguments for the geom functor are in the reverse order (dispatcher would normally call goReverse).
			// we don't remember the fact that is reverse, so we swap bodies within the interaction
			// and can call go in all cases
			if(swap){I->swapOrder();}
			// body pointers must be updated, in case we swapped
			const shared_ptr<Body>& b1=Body::byId(I->getId1(),rootBody);
			const shared_ptr<Body>& b2=Body::byId(I->getId2(),rootBody);

			assert(I->functorCache.geom);
			bool wasReal=I->isReal();
			bool geomCreated=I->functorCache.geom->go(b1->interactingGeometry,b2->interactingGeometry,b1->physicalParameters->se3, b2->physicalParameters->se3,I);
			if(!geomCreated){
				if(wasReal) rootBody->interactions->requestErase(I->getId1(),I->getId2()); // fully created interaction without geometry is reset and perhaps erased in the next step
				continue; // in any case don't care about this one anymore
			}

			// InteractionPhysicsMetaEngine
			if(!I->functorCache.phys){
				I->functorCache.phys=physDispatcher->getFunctor2D(b1->physicalParameters,b2->physicalParameters,swap);
				assert(!swap); // InteractionPhysicsEngineUnits are symmetric
			}
			assert(I->functorCache.phys);
			I->functorCache.phys->go(b1->physicalParameters,b2->physicalParameters,I);
			assert(I->interactionPhysics);
			I->iterMadeReal=rootBody->currentIteration; // mark the interaction as created right now


			// ConstitutiveLawDispatcher
			// populating constLaw cache must be done after geom and physics dispatchers have been called, since otherwise the interaction
			// would not have interactionGeometry and interactionPhysics yet.
			if(!I->functorCache.constLaw){
				I->functorCache.constLaw=constLawDispatcher->getFunctor2D(I->interactionGeometry,I->interactionPhysics,swap);
				assert(!swap); // reverse call would make no sense, as the arguments are of different types
			}
		  	assert(I->functorCache.constLaw);
			I->functorCache.constLaw->go(I->interactionGeometry,I->interactionPhysics,I.get(),rootBody);
		#else
			const shared_ptr<Body>& b1=Body::byId(I->getId1(),rootBody);
			const shared_ptr<Body>& b2=Body::byId(I->getId2(),rootBody);
			// InteractionGeometryMetaEngine
			bool wasReal=I->isReal();
			bool geomCreated =
				b1->interactingGeometry && b2->interactingGeometry && // some bodies do not have interactingGeometry
				geomDispatcher->operator()(b1->interactingGeometry, b2->interactingGeometry, b1->physicalParameters->se3, b2->physicalParameters->se3,I);
			if(!geomCreated){
				if(wasReal) *rootBody->interactions->requestErase(I->getId1(),I->getId2());
				continue;
			}
			// InteractionPhysicsMetaEngine
			// geom may have swapped bodies, get bodies again
			physDispatcher->operator()(Body::byId(I->getId1(),rootBody)->physicalParameters, Body::byId(I->getId2(),rootBody)->physicalParameters,I);
			// ConstitutiveLawDispatcher
			constLawDispatcher->operator()(I->interactionGeometry,I->interactionPhysics,I.get(),rootBody);
		#endif
		}
}
