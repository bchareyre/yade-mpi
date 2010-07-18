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
		#ifdef YADE_DEVIRT_FUNCTORS
			throw logic_error("InteractionGeometryDispatcher::explicitAction not supported with the devirt-functors feature (yet)");
		#endif
		assert(b1->shape && b2->shape);
		shared_ptr<Interaction> I(new Interaction(b1->getId(),b2->getId()));
		// FIXME: this code is more or less duplicated from InteractionDispatchers :-(
		bool swap=false;
		I->functorCache.geom=getFunctor2D(b1->shape,b2->shape,swap);
		if(!I->functorCache.geom) throw invalid_argument("InteractionGeometryDispatcher::explicitAction could not dispatch for given types ("+b1->shape->getClassName()+","+b2->shape->getClassName()+").");
		if(swap){I->swapOrder();}
		const shared_ptr<Body>& b1=Body::byId(I->getId1(),scene);
		const shared_ptr<Body>& b2=Body::byId(I->getId2(),scene);
		bool succ=I->functorCache.geom->go(b1->shape,b2->shape,*b1->state,*b2->state,/*shift2*/Vector3r::Zero(),/*force*/true,I);
		if(!succ) throw logic_error("Functor "+I->functorCache.geom->getClassName()+"::go returned false, even if asked to force InteractionGeometry creation. Please report bug.");
		return I;
	} else {
		shared_ptr<Interaction> I(new Interaction(b1->getId(),b2->getId()));
		b1->shape && b2->shape && operator()( b1->shape , b2->shape , *b1->state , *b2->state , Vector3r::Zero(), /*force*/ false, I);
		return I;
	}
}

void InteractionGeometryDispatcher::action(){
	// Erase interaction that were requested for erase, but not processed by the collider, if any (and warn once about that, as it is suspicious)
	if(scene->interactions->unconditionalErasePending()>0 && !alreadyWarnedNoCollider){
		LOG_WARN("Interactions pending erase found, no collider being used?");
		alreadyWarnedNoCollider=true;
	}
	updateScenePtr();

	shared_ptr<BodyContainer>& bodies = scene->bodies;
	Matrix3r cellHsize; if(scene->isPeriodic) cellHsize=scene->cell->Hsize;
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
				geomCreated=operator()(b1->shape, b2->shape, *b1->state, *b2->state, Vector3r::Zero(), /*force*/ false, I);
			} else{
				Vector3r shift2=cellHsize*I->cellDist.cast<Real>();
				geomCreated=operator()(b1->shape, b2->shape, *b1->state, *b2->state, shift2, /*force*/ false, I);
			}
			// reset && erase interaction that existed but now has no geometry anymore
			if(wasReal && !geomCreated){ scene->interactions->requestErase(I->getId1(),I->getId2()); }
	}
}

YADE_PLUGIN((InteractionGeometryDispatcher));

