/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "PhysicalActionApplier.hpp"
#include<yade/core/MetaBody.hpp>

void PhysicalActionApplier::action(Body* body)
{
	MetaBody * ncb = YADE_CAST<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	PhysicalActionContainer::iterator pai    = ncb->physicalActions->begin();
	PhysicalActionContainer::iterator paiEnd = ncb->physicalActions->end(); 
	for( ; pai!=paiEnd ; ++pai)
	{
		shared_ptr<PhysicalAction> action = *pai;
		int id = pai.getCurrentIndex();
		// FIXME - solve the problem of Body's id
		operator()( action , (*bodies)[id]->physicalParameters , (*bodies)[id].get() );
	}
}


