#include"InteractionDispatchers.hpp"

YADE_PLUGIN((InteractionDispatchers));
CREATE_LOGGER(InteractionDispatchers);

// #define IDISP_TIMING

#ifdef IDISP_TIMING
	#define IDISP_CHECKPOINT(cpt) timingDeltas->checkpoint(cpt)
#else
	#define IDISP_CHECKPOINT(cpt)
#endif




InteractionDispatchers::InteractionDispatchers(){
	geomDispatcher=shared_ptr<InteractionGeometryMetaEngine>(new InteractionGeometryMetaEngine);
	physDispatcher=shared_ptr<InteractionPhysicsMetaEngine>(new InteractionPhysicsMetaEngine);
	constLawDispatcher=shared_ptr<ConstitutiveLawDispatcher>(new ConstitutiveLawDispatcher);
	alreadyWarnedNoCollider=false;
	#ifdef IDISP_TIMING
		timingDeltas=shared_ptr<TimingDeltas>(new TimingDeltas);
	#endif
}

#define DISPATCH_CACHE

void InteractionDispatchers::action(MetaBody* rootBody){
	#ifdef IDISP_TIMING
		timingDeltas->start();
	#endif
	if(rootBody->interactions->unconditionalErasePending()>0 && !alreadyWarnedNoCollider){
		LOG_WARN("Interactions pending erase found (erased), no collider being used?");
		alreadyWarnedNoCollider=true;
	}
	Vector3r cellSize; if(rootBody->isPeriodic) cellSize=rootBody->cellMax-rootBody->cellMin;
	bool removeUnseenIntrs=(rootBody->interactions->iterColliderLastRun>=0 && rootBody->interactions->iterColliderLastRun==rootBody->currentIteration);
	#ifdef YADE_OPENMP
		const long size=rootBody->interactions->size();
		#pragma omp parallel for schedule(guided)
		for(long i=0; i<size; i++){
			const shared_ptr<Interaction>& I=(*rootBody->interactions)[i];
	#else
		FOREACH(shared_ptr<Interaction> I, *rootBody->interactions){
	#endif
		#ifdef DISPATCH_CACHE
			if(removeUnseenIntrs && !I->isReal() && I->iterLastSeen<rootBody->currentIteration) {
				rootBody->interactions->requestErase(I->getId1(),I->getId2());
				continue;
			}

			const shared_ptr<Body>& b1_=Body::byId(I->getId1(),rootBody);
			const shared_ptr<Body>& b2_=Body::byId(I->getId2(),rootBody);

			// FIXME: maybe this could be run even in production code without harm. It would make removing bodies much faster, since we wouldn't have to search its interactions and removing them.
			#ifndef NDEBUG
				if(!b1_ || !b2_){ LOG_ERROR("Body #"<<(b1_?I->getId2():I->getId1())<<" vanished, erasing intr #"<<I->getId1()<<"+#"<<I->getId2()<<"!"); rootBody->interactions->requestErase(I->getId1(),I->getId2(),/*force*/true); continue; }
			#endif

			// go fast if this pair of bodies cannot interact at all
			if((b1_->getGroupMask() & b2_->getGroupMask())==0) continue;

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
			bool geomCreated;
			if(!rootBody->isPeriodic) geomCreated=I->functorCache.geom->go(b1->interactingGeometry,b2->interactingGeometry,b1->physicalParameters->se3, b2->physicalParameters->se3,I);
			else{ // handle periodicity
				Se3r se32=b2->physicalParameters->se3; se32.position+=Vector3r(I->cellDist[0]*cellSize[0],I->cellDist[1]*cellSize[1],I->cellDist[2]*cellSize[2]);
				geomCreated=I->functorCache.geom->go(b1->interactingGeometry,b2->interactingGeometry,b1->physicalParameters->se3,se32,I);
			}
			if(!geomCreated){
				if(wasReal) rootBody->interactions->requestErase(I->getId1(),I->getId2()); // fully created interaction without geometry is reset and perhaps erased in the next step
				continue; // in any case don't care about this one anymore
			}


			// InteractionPhysicsMetaEngine
			if(!I->functorCache.phys){
				I->functorCache.phys=physDispatcher->getFunctor2D(b1->physicalParameters,b2->physicalParameters,swap);
				assert(!swap); // InteractionPhysicsEngineUnits are symmetric
			}
			//assert(I->functorCache.phys);
			if(!I->functorCache.phys){
				throw std::runtime_error("Undefined or ambiguous InteractionPhysics dispatch for types "+b1->physicalParameters->getClassName()+" and "+b2->physicalParameters->getClassName()+".");
			}
			I->functorCache.phys->go(b1->physicalParameters,b2->physicalParameters,I);
			assert(I->interactionPhysics);

			if(!wasReal) I->iterMadeReal=rootBody->currentIteration; // mark the interaction as created right now

			// ConstitutiveLawDispatcher
			// populating constLaw cache must be done after geom and physics dispatchers have been called, since otherwise the interaction
			// would not have interactionGeometry and interactionPhysics yet.
			if(!I->functorCache.constLaw){
				I->functorCache.constLaw=constLawDispatcher->getFunctor2D(I->interactionGeometry,I->interactionPhysics,swap);
				if(!I->functorCache.constLaw){
					LOG_FATAL("getFunctor2D returned empty functor for  #"<<I->getId1()<<"+"<<I->getId2()<<", types "<<I->interactionGeometry->getClassName()<<"="<<I->interactionGeometry->getClassIndex()<<" and "<<I->interactionPhysics->getClassName()<<"="<<I->interactionPhysics->getClassIndex());
					//abort();
					exit(1);
				}
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

YADE_REQUIRE_FEATURE(PHYSPAR);

