/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "PhysicalActionDamper.hpp"
#include<yade/core/MetaBody.hpp>

#ifdef BEX_CONTAINER
void PhysicalActionDamper::action(MetaBody* ncb){
	FOREACH(const shared_ptr<Body>& b, *ncb->bodies){
		operator()(b->physicalParameters,b.get(),ncb);
	}
}
#else
void PhysicalActionDamper::action(MetaBody* ncb)
{
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
#endif


YADE_PLUGIN();
