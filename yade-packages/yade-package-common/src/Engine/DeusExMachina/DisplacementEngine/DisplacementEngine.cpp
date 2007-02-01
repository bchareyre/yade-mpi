/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "DisplacementEngine.hpp"
#include <yade/yade-core/MetaBody.hpp>

void DisplacementEngine::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		translationAxis.Normalize();
		/*
		if(displacement==0)
			displacement=2e-10;
		else
			displacement=0;
		*/
		std::cerr << "displacement: " << displacement << "\n";
	}
}


void DisplacementEngine::registerAttributes()
{
	DeusExMachina::registerAttributes();
	REGISTER_ATTRIBUTE(displacement);
	REGISTER_ATTRIBUTE(translationAxis);
}


void DisplacementEngine::applyCondition(Body * body)
{
	MetaBody * ncb = static_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	std::vector<int>::const_iterator ii = subscribedBodies.begin();
	std::vector<int>::const_iterator iiEnd = subscribedBodies.end();

//
// FIXME - we really need to set intervals for engines.
//

//	if(Omega::instance().getCurrentIteration() > 2000)
//		return;


//      {


	for(;ii!=iiEnd;++ii)
		if( bodies->exists(*ii) )
			((*bodies)[*ii]->physicalParameters.get())->se3.position += displacement*translationAxis;
		
		
//	}
//	else
//	{
//	for(;ii!=iiEnd;++ii)
//		((*bodies)[*ii])->isDynamic = true;
//	}

}

