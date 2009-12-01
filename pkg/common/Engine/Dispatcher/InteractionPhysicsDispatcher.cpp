/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractionPhysicsDispatcher.hpp"


#include<yade/core/World.hpp>
/* Single dispatch for given bodies pp's and interaction.
 * 
 * The interaction must be real (needed?).
 */
void InteractionPhysicsDispatcher::explicitAction(shared_ptr<Material>& pp1, shared_ptr<Material>& pp2, shared_ptr<Interaction>& i){
	// should we throw instead of asserting?
	assert(i->isReal());
	operator()(pp1,pp2,i);
}


void InteractionPhysicsDispatcher::action(World* ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	#ifdef YADE_OPENMP
		const long size=ncb->interactions->size();
		#pragma omp parallel for
		for(long i=0; i<size; i++){
			const shared_ptr<Interaction>& interaction=(*ncb->interactions)[i];
	#else
		FOREACH(const shared_ptr<Interaction>& interaction, *ncb->interactions){
	#endif
			if(interaction->interactionGeometry){
				shared_ptr<Body>& b1 = (*bodies)[interaction->getId1()];
				shared_ptr<Body>& b2 = (*bodies)[interaction->getId2()];
				bool hadPhys=interaction->interactionPhysics;
				operator()(b1->material, b2->material, interaction);
				assert(interaction->interactionPhysics);
				if(!hadPhys) interaction->iterMadeReal=ncb->currentIteration;
			}
		}
}

YADE_PLUGIN((InteractionPhysicsDispatcher));
