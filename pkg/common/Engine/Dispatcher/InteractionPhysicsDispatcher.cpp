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
void InteractionPhysicsDispatcher::explicitAction(shared_ptr<Material>& pp1, shared_ptr<Material>& pp2, shared_ptr<Interaction>& i){
	// should we throw instead of asserting?
	//assert(i->isReal());
	updateScenePtr();
	if(!i->interactionGeometry) throw runtime_error(string(__FILE__)+": explicitAction received interaction without interactionGeometry.");
	operator()(pp1,pp2,i);
}


void InteractionPhysicsDispatcher::action(Scene*)
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
