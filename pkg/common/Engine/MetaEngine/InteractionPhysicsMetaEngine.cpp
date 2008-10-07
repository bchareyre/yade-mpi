/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractionPhysicsMetaEngine.hpp"


#include<yade/core/MetaBody.hpp>
/* Single dispatch for given bodies pp's and interaction.
 * 
 * The interaction must be real (needed?).
 */
void InteractionPhysicsMetaEngine::explicitAction(shared_ptr<PhysicalParameters>& pp1, shared_ptr<PhysicalParameters>& pp2, shared_ptr<Interaction>& i){
	// should we throw instead of asserting?
	assert(i->isReal);
	operator()(pp1,pp2,i);
}


void InteractionPhysicsMetaEngine::action(MetaBody* ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	shared_ptr<InteractionContainer>& persistentInteractions = ncb->persistentInteractions;
	InteractionContainer::iterator ii    = persistentInteractions->begin();
	InteractionContainer::iterator iiEnd = persistentInteractions->end(); 
	for( ; ii!=iiEnd ; ++ii)
	{
		const shared_ptr<Interaction> interaction = *ii;

		shared_ptr<Body>& b1 = (*bodies)[interaction->getId1()];
		shared_ptr<Body>& b2 = (*bodies)[interaction->getId2()];
		if( b1->physicalParameters && b2->physicalParameters )
			operator()( b1->physicalParameters , b2->physicalParameters , interaction );
	}

	shared_ptr<InteractionContainer>& transientInteractions = ncb->transientInteractions;
	ii    = transientInteractions->begin();
	iiEnd = transientInteractions->end(); 
	for( ; ii!=iiEnd ; ++ii)
	{
		const shared_ptr<Interaction> interaction = *ii;
		
		shared_ptr<Body>& b1 = (*bodies)[interaction->getId1()];
		shared_ptr<Body>& b2 = (*bodies)[interaction->getId2()];

		if (interaction->isReal)
			operator()( b1->physicalParameters , b2->physicalParameters , interaction );
	}
}

YADE_PLUGIN();
