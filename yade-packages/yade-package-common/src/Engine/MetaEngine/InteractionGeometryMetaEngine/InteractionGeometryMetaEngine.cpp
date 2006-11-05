/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractionGeometryMetaEngine.hpp"


#include <yade/yade-core/MetaBody.hpp>


void InteractionGeometryMetaEngine::action(Body* body)
{
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	shared_ptr<InteractionContainer>& persistentInteractions = ncb->persistentInteractions;
	InteractionContainer::iterator ii    = persistentInteractions->begin();
	InteractionContainer::iterator iiEnd = persistentInteractions->end(); 
	for( ; ii!=iiEnd ; ++ii)
	{
		const shared_ptr<Interaction> interaction = *ii;
		
		shared_ptr<Body>& b1 = (*bodies)[interaction->getId1()];
		shared_ptr<Body>& b2 = (*bodies)[interaction->getId2()];
		interaction->isReal = true;
		operator()( b1->interactingGeometry , b2->interactingGeometry , b1->physicalParameters->se3 , b2->physicalParameters->se3 , interaction );
	}
	
	shared_ptr<InteractionContainer>& volatileInteractions = ncb->volatileInteractions;
	ii    = volatileInteractions->begin();
	iiEnd = volatileInteractions->end(); 
	for(  ; ii!=iiEnd ; ++ii)
	{
		const shared_ptr<Interaction> interaction = *ii;
		
		shared_ptr<Body>& b1 = (*bodies)[interaction->getId1()];
		shared_ptr<Body>& b2 = (*bodies)[interaction->getId2()];
		
		interaction->isReal =

		// FIXME put this inside VolatileInteractionCriterion dynlib
			( persistentInteractions->find(interaction->getId1(),interaction->getId2()) == 0 )
		 	&&
			operator()( b1->interactingGeometry , b2->interactingGeometry , b1->physicalParameters->se3 , b2->physicalParameters->se3 , interaction );
			
	}
}

