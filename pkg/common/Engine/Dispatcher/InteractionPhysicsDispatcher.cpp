/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractionPhysicsDispatcher.hpp"


#include<yade/core/Scene.hpp>
/* Single dispatch for given bodies pp's and interaction.
 * 
 * The interaction must be real (needed?).
 */
void InteractionPhysicsDispatcher::explicitAction(shared_ptr<Material>& pp1, shared_ptr<Material>& pp2, shared_ptr<Interaction>& I){
	// should we throw instead of asserting?
	//assert(i->isReal());
	updateScenePtr();
	if(!I->interactionGeometry) throw invalid_argument(string(__FILE__)+": explicitAction received interaction without interactionGeometry.");
	if(!I->functorCache.phys){
		bool dummy;
		I->functorCache.phys=getFunctor2D(pp1,pp2,dummy);
		if(!I->functorCache.phys) throw invalid_argument("InteractionPhysicsDispatcher::explicitAction did not find a suitable dispatch for types "+pp1->getClassName()+" and "+pp2->getClassName());
		I->functorCache.phys->go(pp1,pp2,I);
	}
}


void InteractionPhysicsDispatcher::action()
{
	updateScenePtr();
	shared_ptr<BodyContainer>& bodies = scene->bodies;
	#ifdef YADE_OPENMP
		const long size=scene->interactions->size();
		#pragma omp parallel for
		for(long i=0; i<size; i++){
			const shared_ptr<Interaction>& interaction=(*scene->interactions)[i];
	#else
		FOREACH(const shared_ptr<Interaction>& interaction, *scene->interactions){
	#endif
			if(interaction->interactionGeometry){
				shared_ptr<Body>& b1 = (*bodies)[interaction->getId1()];
				shared_ptr<Body>& b2 = (*bodies)[interaction->getId2()];
				bool hadPhys=interaction->interactionPhysics;
				operator()(b1->material, b2->material, interaction);
				assert(interaction->interactionPhysics);
				if(!hadPhys) interaction->iterMadeReal=scene->currentIteration;
			}
		}
}

YADE_PLUGIN((InteractionPhysicsDispatcher));
