/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNUGeneral Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractionGeometryMetaEngine.hpp"


#include<yade/core/MetaBody.hpp>
	
/* Single dispatch for given pair of bodies, returning new interaction.
 *
 * The caller is responsible for inserting the interaction into some interaction container.
 *
 * The EngineUnit must not fail (return false).
 */

shared_ptr<Interaction> InteractionGeometryMetaEngine::explicitAction(const shared_ptr<Body>& b1, const shared_ptr<Body>& b2){
	//assert(b1->interactingGeometry && b2->interactingGeometry);
	//shared_ptr<Interaction> i(new Interaction(b1->getId(),b2->getId()));
	//i->isReal=true;
	//bool op=operator()(b1->interactingGeometry,b2->interactingGeometry,b1->physicalParameters->se3,b2->physicalParameters->se3,i);
	//if(!op) throw runtime_error("InteractionGeometryMetaEngine::explicitAction could not dispatch for given types ("+b1->interactingGeometry->getClassName()+","+b2->interactingGeometry->getClassName()+") or the dispatchee returned false.");
	//return i;
	
	// Seems asserts and throws in code above is not good idea.
	// Below code do same (i.e. create interaction for specified bodies), but
	// without artifical exceptions. If creating interaction is fail (for
	// example if bodies don't have an interactionGeometry), returned
	// interaction is non real, i.e. interaction->isReal==false. Sega.
	shared_ptr<Interaction> interaction(new Interaction(b1->getId(),b2->getId()));
	interaction->isReal =
		b1->interactingGeometry && b2->interactingGeometry && 
		operator()( b1->interactingGeometry , b2->interactingGeometry , b1->physicalParameters->se3 , b2->physicalParameters->se3 , interaction );
	return interaction;
}

void InteractionGeometryMetaEngine::action(MetaBody* ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	#ifdef YADE_OPENMP
		const long size=ncb->transientInteractions->size();
		#pragma omp parallel for
		for(long i=0; i<size; i++){
			const shared_ptr<Interaction>& interaction=(*ncb->transientInteractions)[i];
	#else
		FOREACH(const shared_ptr<Interaction>& interaction, *ncb->interactions){
	#endif
			const shared_ptr<Body>& b1=(*bodies)[interaction->getId1()];
			const shared_ptr<Body>& b2=(*bodies)[interaction->getId2()];
			interaction->isReal =
				b1->interactingGeometry && b2->interactingGeometry && // some bodies do not have interactingGeometry
				operator()(b1->interactingGeometry, b2->interactingGeometry, b1->physicalParameters->se3, b2->physicalParameters->se3, interaction);
	}
}

YADE_PLUGIN();
