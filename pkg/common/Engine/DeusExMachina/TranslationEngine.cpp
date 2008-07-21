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

	std::vector<int>::const_iterator ii = subscribedBodies.begin();
	std::vector<int>::const_iterator iiEnd = subscribedBodies.end();

	Real dt=Omega::instance().getTimeStep();
	//time=dt;
	static int sign = 1;
	for(;ii!=iiEnd;++ii){
		if(ParticleParameters* p = dynamic_cast<ParticleParameters*>((*bodies)[*ii]->physicalParameters.get())){
			p->se3.position+=sign*dt*velocity*translationAxis;
			p->velocity=sign*velocity*translationAxis;
		} else if(PhysicalParameters* b = dynamic_cast<PhysicalParameters*>((*bodies)[*ii]->physicalParameters.get())){
			// NOT everyone has velocity !
			b->se3.position+=sign*dt*velocity*translationAxis;
		} else {
			std::cerr << "TranslationEngine::applyCondition, WARNING! dynamic_cast failed! for id: " << *ii << std::endl;
		}
	}
}

YADE_PLUGIN();
