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
	geomDispatcher=shared_ptr<InteractionGeometryDispatcher>(new InteractionGeometryDispatcher);
	physDispatcher=shared_ptr<InteractionPhysicsDispatcher>(new InteractionPhysicsDispatcher);
	lawDispatcher=shared_ptr<LawDispatcher>(new LawDispatcher);
	alreadyWarnedNoCollider=false;
	#ifdef IDISP_TIMING
		timingDeltas=shared_ptr<TimingDeltas>(new TimingDeltas);
	#endif
}

#define DISPATCH_CACHE

void InteractionDispatchers::action(Scene*){
	#ifdef IDISP_TIMING
		timingDeltas->start();
	#endif
	if(scene->interactions->unconditionalErasePending()>0 && !alreadyWarnedNoCollider){
		LOG_WARN("Interactions pending erase found (erased), no collider being used?");
		alreadyWarnedNoCollider=true;
	}
	geomDispatcher->updateScenePtr();
	physDispatcher->updateScenePtr();
	lawDispatcher->updateScenePtr();
	Vector3r cellSize; if(scene->isPeriodic) cellSize=scene->cellSize;
	bool removeUnseenIntrs=(scene->interactions->iterColliderLastRun>=0 && scene->interactions->iterColliderLastRun==scene->currentIteration);
	#ifdef YADE_OPENMP
		const long size=scene->interactions->size();
		#pragma omp parallel for schedule(guided)
		for(long i=0; i<size; i++){
			const shared_ptr<Interaction>& I=(*scene->interactions)[i];
	#else
		FOREACH(shared_ptr<Interaction> I, *scene->interactions){
	#endif
		#ifdef DISPATCH_CACHE
			if(removeUnseenIntrs && !I->isReal() && I->iterLastSeen<scene->currentIteration) {
				scene->interactions->requestErase(I->getId1(),I->getId2());
				continue;
			}

			const shared_ptr<Body>& b1_=Body::byId(I->getId1(),scene);
			const shared_ptr<Body>& b2_=Body::byId(I->getId2(),scene);

			if(!b1_ || !b2_){ LOG_DEBUG("Body #"<<(b1_?I->getId2():I->getId1())<<" vanished, erasing intr #"<<I->getId1()<<"+#"<<I->getId2()<<"!"); scene->interactions->requestErase(I->getId1(),I->getId2(),/*force*/true); continue; }

			// already in Collider::mayCollider, no need to check here anymore
			#if 0
				// go fast if this pair of bodies cannot interact at all
				if((b1_->getGroupMask() & b2_->getGroupMask())==0) continue;
			#endif

			// we know there is no geometry functor already, take the short path
			if(!I->functorCache.geomExists) { assert(!I->isReal()); continue; }
			// no interaction geometry for either of bodies; no interaction possible
			if(!b1_->shape || !b2_->shape) { assert(!I->isReal()); continue; }

			bool swap=false;
			// InteractionGeometryDispatcher
			if(!I->functorCache.geom || !I->functorCache.phys){
				I->functorCache.geom=geomDispatcher->getFunctor2D(b1_->shape,b2_->shape,swap);
				// returns NULL ptr if no functor exists; remember that and shortcut
				if(!I->functorCache.geom) { I->functorCache.geomExists=false; continue; }
			}
			// arguments for the geom functor are in the reverse order (dispatcher would normally call goReverse).
			// we don't remember the fact that is reverse, so we swap bodies within the interaction
			// and can call go in all cases
			if(swap){I->swapOrder();}
			// body pointers must be updated, in case we swapped
			const shared_ptr<Body>& b1=Body::byId(I->getId1(),scene);
			const shared_ptr<Body>& b2=Body::byId(I->getId2(),scene);

			assert(I->functorCache.geom);
			bool wasReal=I->isReal();
			bool geomCreated;
			if(!scene->isPeriodic) geomCreated=I->functorCache.geom->go(b1->shape,b2->shape, *b1->state, *b2->state, Vector3r::ZERO, /*force*/false, I);
			else{ // handle periodicity
				Vector3r shift2(I->cellDist[0]*cellSize[0],I->cellDist[1]*cellSize[1],I->cellDist[2]*cellSize[2]);
				geomCreated=I->functorCache.geom->go(b1->shape,b2->shape,*b1->state,*b2->state,shift2,/*force*/false,I);
			}
			if(!geomCreated){
				if(wasReal) scene->interactions->requestErase(I->getId1(),I->getId2()); // fully created interaction without geometry is reset and perhaps erased in the next step
				continue; // in any case don't care about this one anymore
			}

			// InteractionPhysicsDispatcher
			if(!I->functorCache.phys){
				I->functorCache.phys=physDispatcher->getFunctor2D(b1->material,b2->material,swap);
				assert(!swap); // InteractionPhysicsEngineUnits are symmetric
			}
			//assert(I->functorCache.phys);
			if(!I->functorCache.phys){
				throw std::runtime_error("Undefined or ambiguous InteractionPhysics dispatch for types "+b1->material->getClassName()+" and "+b2->material->getClassName()+".");
			}
			I->functorCache.phys->go(b1->material,b2->material,I);
			assert(I->interactionPhysics);

			if(!wasReal) I->iterMadeReal=scene->currentIteration; // mark the interaction as created right now

			// LawDispatcher
			// populating constLaw cache must be done after geom and physics dispatchers have been called, since otherwise the interaction
			// would not have interactionGeometry and interactionPhysics yet.
			if(!I->functorCache.constLaw){
				I->functorCache.constLaw=lawDispatcher->getFunctor2D(I->interactionGeometry,I->interactionPhysics,swap);
				if(!I->functorCache.constLaw){
					LOG_FATAL("getFunctor2D returned empty functor for  #"<<I->getId1()<<"+"<<I->getId2()<<", types "<<I->interactionGeometry->getClassName()<<"="<<I->interactionGeometry->getClassIndex()<<" and "<<I->interactionPhysics->getClassName()<<"="<<I->interactionPhysics->getClassIndex());
					//abort();
					exit(1);
				}
				assert(!swap); // reverse call would make no sense, as the arguments are of different types
			}
		  	assert(I->functorCache.constLaw);
			I->functorCache.constLaw->go(I->interactionGeometry,I->interactionPhysics,I.get(),scene);
		#else
			const shared_ptr<Body>& b1=Body::byId(I->getId1(),scene);
			const shared_ptr<Body>& b2=Body::byId(I->getId2(),scene);
			// InteractionGeometryDispatcher
			bool wasReal=I->isReal();
			bool geomCreated =
				b1->shape && b2->shape && // some bodies do not have shape
				geomDispatcher->operator()(b1->shape, b2->shape, *b1->state, *b2->state, Vector3r::ZERO, I);
			// FIXME: port from the part above
			if(scene->isPeriodic) { LOG_FATAL(__FILE__ ": Periodicity not handled without DISPATCH_CACHE."); abort(); }
			if(!geomCreated){
				if(wasReal) *scene->interactions->requestErase(I->getId1(),I->getId2());
				continue;
			}
			// InteractionPhysicsDispatcher
			// geom may have swapped bodies, get bodies again
			physDispatcher->operator()(Body::byId(I->getId1(),scene)->material, Body::byId(I->getId2(),scene)->material,I);
			// LawDispatcher
			lawDispatcher->operator()(I->interactionGeometry,I->interactionPhysics,I.get(),scene);
		#endif
		}
}
