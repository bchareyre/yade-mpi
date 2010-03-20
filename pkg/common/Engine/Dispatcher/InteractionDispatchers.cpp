#include"InteractionDispatchers.hpp"

YADE_PLUGIN((InteractionDispatchers));
CREATE_LOGGER(InteractionDispatchers);

//! Pseudo-ctor for InteractionDispatchers, using lists of functors (might be turned into real ctor, perhaps)
shared_ptr<InteractionDispatchers> InteractionDispatchers_ctor_lists(const std::vector<shared_ptr<InteractionGeometryFunctor> >& gff, const std::vector<shared_ptr<InteractionPhysicsFunctor> >& pff, const std::vector<shared_ptr<LawFunctor> >& cff /*, const std::vector<shared_ptr<IntrCallback> >& cbs*/){
	shared_ptr<InteractionDispatchers> instance(new InteractionDispatchers);
	FOREACH(shared_ptr<InteractionGeometryFunctor> gf, gff) instance->geomDispatcher->add(gf);
	FOREACH(shared_ptr<InteractionPhysicsFunctor> pf, pff) instance->physDispatcher->add(pf);
	FOREACH(shared_ptr<LawFunctor> cf, cff) instance->lawDispatcher->add(cf);
	// FOREACH(shared_ptr<IntrCallback> cb, cbs) instance->callbacks.push_back(cb);
	return instance;
}


// #define IDISP_TIMING

#ifdef IDISP_TIMING
	#define IDISP_CHECKPOINT(cpt) timingDeltas->checkpoint(cpt)
#else
	#define IDISP_CHECKPOINT(cpt)
#endif

#define DISPATCH_CACHE

void InteractionDispatchers::action(){
	#ifdef IDISP_TIMING
		timingDeltas->start();
	#endif
	if(scene->interactions->unconditionalErasePending()>0 && !alreadyWarnedNoCollider){
		LOG_WARN("Interactions pending erase found (erased), no collider being used?");
		alreadyWarnedNoCollider=true;
	}
	// update Scene* of the dispatchers
	geomDispatcher->scene=physDispatcher->scene=lawDispatcher->scene=scene;
	// ask dispatchers to update Scene* of their functors
	geomDispatcher->updateScenePtr(); physDispatcher->updateScenePtr(); lawDispatcher->updateScenePtr();
	/*
		initialize callbacks; they return pointer (used only in this timestep) to the function to be called
		returning NULL deactivates the callback in this timestep
	*/
	// pair of callback object and pointer to the function to be called
	vector<IntrCallback::FuncPtr> callbackPtrs;
	FOREACH(const shared_ptr<IntrCallback> cb, callbacks){
		cb->scene=scene;
		callbackPtrs.push_back(cb->stepInit());
	}
	assert(callbackPtrs.size()==callbacks.size());
	size_t callbacksSize=callbacks.size();

	// precompute transformed cell size 
	Vector3r cellSize; if(scene->isPeriodic) cellSize=scene->cell->trsf*scene->cell->refSize;

	// force removal of interactions that were not encountered by the collider
	// (only for some kinds of colliders; see comment for InteractionContainer::iterColliderLastRun)
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

				// in sheared cell, apply shear on the mutual position as well
				shift2=scene->cell->shearPt(shift2);
				/* suggested change to avoid one matrix multiplication (with Hsize updated in cell ofc), I'll make the change cleanly if ok.
					Same sorts of simplifications are possible in many places. Just putting one example here.
					Hsize will contain colums with transformed base vectors
					Matrix3r Hsize(scene->cell->refSize[0],scene->cell->refSize[1],scene->cell->refSize[2]); Hsize=scene->cell->trsf*Hsize;
					Vector3r shift3((Real) I->cellDist[0]*Hsize.GetColumn(0)+(Real)I->cellDist[1]*Hsize.GetColumn(1)+(Real)I->cellDist[2]*Hsize.GetColumn(2));
					if ((Omega::instance().getCurrentIteration() % 100 == 0)) LOG_DEBUG(shift2 << " vs " << shift3);
				*/

				geomCreated=I->functorCache.geom->go(b1->shape,b2->shape,*b1->state,*b2->state,shift2,/*force*/false,I);
			}
			if(!geomCreated){
				if(wasReal) LOG_WARN("InteractionGeometryFunctor returned false on existing interaction!");
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
			if(!I->isReal() && I->isFresn(scene)) LOG_WARN("Law functor deleted interaction that was just created. Please report bug: either this message is spurious, or the functor (or something else) is buggy.");
		#endif

		// process callbacks for this interaction
		if(!I->isReal()) continue; // it is possible that Law2_ functor called requestErase, hence this check
		for(size_t i=0; i<callbacksSize; i++){
			if(callbackPtrs[i]!=NULL) (*(callbackPtrs[i]))(callbacks[i].get(),I.get());
		}
	}
}
