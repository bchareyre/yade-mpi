/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "TranslationEngine.hpp"
#include<yade/pkg-common/ParticleParameters.hpp>
#include<yade/core/MetaBody.hpp>

void TranslationEngine::postProcessAttributes(bool deserializing)
{
	if(deserializing)
		translationAxis.Normalize();
}


void TranslationEngine::registerAttributes()
{
	DeusExMachina::registerAttributes();
	REGISTER_ATTRIBUTE(velocity);
	REGISTER_ATTRIBUTE(translationAxis);
}


void TranslationEngine::applyCondition(MetaBody * ncb){
	shared_ptr<BodyContainer>& bodies=ncb->bodies;

	Real dt=Omega::instance().getTimeStep();
	static int sign = 1;
	FOREACH(body_id_t id,subscribedBodies){
		assert(id<bodies->size());
		if(ParticleParameters* p = dynamic_cast<ParticleParameters*>((*bodies)[id]->physicalParameters.get())){
			p->se3.position+=sign*dt*velocity*translationAxis;
			p->velocity=sign*velocity*translationAxis;
		} else{
			Body::byId(id,ncb)->physicalParameters->se3.position+=sign*dt*velocity*translationAxis;
		}
	}
}

YADE_PLUGIN();
