/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"PhysicalActionApplier.hpp"
#include<yade/core/MetaBody.hpp>

#ifdef BEX_CONTAINER
void PhysicalActionApplier::action(MetaBody* ncb){
	FOREACH(const shared_ptr<Body>& b, *ncb->bodies){
		operator()(b->physicalParameters,b.get(),ncb);
	}
}
#else
void PhysicalActionApplier::action(MetaBody* ncb)
{
		throw logic_error("PhysicalActionApplier cannot be used with BexContainer. Use NewtonsDampedLaw instead (or recompile with NO_BEX).");
		shared_ptr<BodyContainer>& bodies = ncb->bodies;

		PhysicalActionContainer::iterator pai    = ncb->physicalActions->begin();
		PhysicalActionContainer::iterator paiEnd = ncb->physicalActions->end(); 
		for( ; pai!=paiEnd ; ++pai)
		{
			shared_ptr<PhysicalAction> action = *pai;
			int id = pai.getCurrentIndex();
			operator()( action , (*bodies)[id]->physicalParameters , (*bodies)[id].get() );
		}
}
#endif


YADE_PLUGIN();
