/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNUGeneral Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractionGeometryDispatcher.hpp"


#include<yade/core/Scene.hpp>
	
CREATE_LOGGER(InteractionGeometryDispatcher);

/* Single dispatch for given pair of bodies, returning new interaction.
 *
 * The caller is responsible for inserting the interaction into some interaction container.
 *
 * The Functor must not fail (return false).
 *
 * \fixme: doesn't handle periodicity!
 */

shared_ptr<Interaction> InteractionGeometryDispatcher::explicitAction(const shared_ptr<Body>& b1, const shared_ptr<Body>& b2, bool force){
	updateScenePtr();
	if(force){
		assert(b1->shape && b2->shape);
		shared_ptr<Interaction> i(new Interaction(b1->getId(),b2->getId()));
		bool op=operator()(b1->shape,b2->shape,*b1->state,*b2->state,/*shift2*/Vector3r::ZERO,/*force*/true,i);
		if(!op) throw runtime_error("InteractionGeometryDispatcher::explicitAction could not dispatch for given types ("+b1->shape->getClassName()+","+b2->shape->getClassName()+") or the dispatchee returned false (it was asked to force creation of InteractionGeometry; that would a bug).");
		return i;
	} else {
		shared_ptr<Interaction> interaction(new Interaction(b1->getId(),b2->getId()));
		b1->shape && b2->shape && operator()( b1->shape , b2->shape , *b1->state , *b2->state , Vector3r::ZERO, /*force*/ false, interaction );
		return interaction;
	}
}

void InteractionGeometryDispatcher::action(Scene*){
	// Erase interaction that were requested for erase, but not processed by the collider, if any (and warn once about that, as it is suspicious)
	if(scene->interactions->unconditionalErasePending()>0 && !alreadyWarnedNoCollider){
		LOG_WARN("Interactions pending erase found, no collider being used?");
		alreadyWarnedNoCollider=true;
	}
	updateScenePtr();

	shared_ptr<BodyContainer>& bodies = scene->bodies;
	Vector3r cellSize; if(scene->isPeriodic) cellSize=scene->cellMax-scene->cellMin;
	bool removeUnseenIntrs=(scene->interactions->iterColliderLastRun>=0 && scene->interactions->iterColliderLastRun==scene->currentIteration);
	#ifdef YADE_OPENMP
		const long size=scene->interactions->size();
		#pragma omp parallel for
		for(long i=0; i<size; i++){
			const shared_ptr<Interaction>& I=(*scene->interactions)[i];
	#else
		FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
	#endif
			if(removeUnseenIntrs && !I->isReal() && I->iterLastSeen<scene->currentIteration) {
				scene->interactions->requestErase(I->getId1(),I->getId2());
				continue;
			}
			const shared_ptr<Body>& b1=(*bodies)[I->getId1()];
			const shared_ptr<Body>& b2=(*bodies)[I->getId2()];

			if(!b1 || !b2){ LOG_DEBUG("Body #"<<(b1?I->getId2():I->getId1())<<" vanished, erasing intr #"<<I->getId1()<<"+#"<<I->getId2()<<"!"); scene->interactions->requestErase(I->getId1(),I->getId2(),/*force*/true); continue; }

			bool wasReal=I->isReal();
			if (!b1->shape || !b2->shape) { assert(!wasReal); continue; } // some bodies do not have shape
			bool geomCreated;
			if(!scene->isPeriodic){
				geomCreated=operator()(b1->shape, b2->shape, *b1->state, *b2->state, Vector3r::ZERO, /*force*/ false, I);
			} else{
				Vector3r shift2(I->cellDist[0]*cellSize[0],I->cellDist[1]*cellSize[1],I->cellDist[2]*cellSize[2]); // add periodicity to the position of the 2nd body
				geomCreated=operator()(b1->shape, b2->shape, *b1->state, *b2->state, shift2, /*force*/ false, I);
			}
			// reset && erase interaction that existed but now has no geometry anymore
			if(wasReal && !geomCreated){ scene->interactions->requestErase(I->getId1(),I->getId2()); }
	}
}

YADE_PLUGIN((InteractionGeometryDispatcher));

