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
	
/* Single dispatch for given pair of bodies, returning new interaction.
 *
 * The caller is responsible for inserting the interaction into some interaction container.
 *
 * The EngineUnit must not fail (return false).
 */

shared_ptr<Interaction> InteractionGeometryMetaEngine::explicitAction(const shared_ptr<Body>& b1, const shared_ptr<Body> b2){
	assert(b1->interactingGeometry && b2->interactingGeometry);
	shared_ptr<Interaction> i(new Interaction(b1->getId(),b2->getId()));
	i->isReal=true;
	bool op=operator()(b1->interactingGeometry,b2->interactingGeometry,b1->physicalParameters->se3,b2->physicalParameters->se3,i);
	if(!op) throw runtime_error("InteractionGeometryMetaEngine::explicitAction could not dispatch for given types ("+b1->interactingGeometry->getClassName()+","+b2->interactingGeometry->getClassName()+") or the dispatchee returned false.");
	return i;
}

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
