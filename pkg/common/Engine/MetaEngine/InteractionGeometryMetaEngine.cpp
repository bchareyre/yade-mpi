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


#include<yade/core/MetaBody.hpp>


void InteractionGeometryMetaEngine::action(MetaBody* ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	FOREACH(const shared_ptr<Interaction>& interaction, *ncb->persistentInteractions){
		shared_ptr<Body>& b1=(*bodies)[interaction->getId1()];
		shared_ptr<Body>& b2=(*bodies)[interaction->getId2()];
		interaction->isReal = true;
		operator()( b1->interactingGeometry , b2->interactingGeometry , b1->physicalParameters->se3 , b2->physicalParameters->se3 , interaction );
	}
	
	FOREACH(const shared_ptr<Interaction>& interaction, *ncb->transientInteractions){
		const shared_ptr<Body>& b1=(*bodies)[interaction->getId1()];
		const shared_ptr<Body>& b2=(*bodies)[interaction->getId2()];
		//bool wasReal = interaction->isReal;
		interaction->isReal =
			b1->interactingGeometry && b2->interactingGeometry && // some bodies do not have interactingGeometry
			( ncb->persistentInteractions->find(interaction->getId1(),interaction->getId2()) == 0 )
		 	&&
			operator()( b1->interactingGeometry , b2->interactingGeometry , b1->physicalParameters->se3 , b2->physicalParameters->se3 , interaction );

		//if(wasReal==false && interaction->isReal)
		//	interaction->isNew=true;
		//cerr<<"isReal="<<interaction->isReal<<", wasReal="<<wasReal<<", isNew="<<interaction->isNew<<endl;

		//tmp
		if(!(b1->interactingGeometry&&b2->interactingGeometry)){
			cerr<<__FILE__<<":"<<__LINE__<<": no interacting geometry "<< (b1->interactingGeometry?b1->getId():-1)<<" "<<(b2->interactingGeometry?b2->getId():-1)<<endl;
		}
			
	}
}

YADE_PLUGIN();
