/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "FixedPositionEngine.hpp"
#include<yade/core/MetaBody.hpp>

FixedPositionEngine::FixedPositionEngine()
{
	fixedPosition=Vector3r(0,0,0);
	mask=Vector3r(0,0,1);
}

void FixedPositionEngine::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		std::cerr << "fixedPosition: " << fixedPosition << "\n";
		std::cerr << "mask: " << mask << "\n\n";
	}
}


void FixedPositionEngine::registerAttributes()
{
//	DeusExMachina::registerAttributes();
	REGISTER_ATTRIBUTE(fixedPosition);
	REGISTER_ATTRIBUTE(mask);
}


void FixedPositionEngine::applyCondition(Body * body)
{
	MetaBody * ncb = static_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

//	std::vector<int>::const_iterator ii = subscribedBodies.begin();
//	std::vector<int>::const_iterator iiEnd = subscribedBodies.end();
//
//	for(;ii!=iiEnd;++ii)
//		if( bodies->exists(*ii) )
//		{
//			if(mask[0] != 0)  ((*bodies)[*ii]->physicalParameters.get())->se3.position[0] = fixedPosition[0];
//			if(mask[1] != 0)  ((*bodies)[*ii]->physicalParameters.get())->se3.position[1] = fixedPosition[1];
//			if(mask[2] != 0)  ((*bodies)[*ii]->physicalParameters.get())->se3.position[2] = fixedPosition[2];
//		}

	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for( ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> b = *bi;
		if(mask[0] != 0)  (b->physicalParameters.get())->se3.position[0] = fixedPosition[0];
		if(mask[1] != 0)  (b->physicalParameters.get())->se3.position[1] = fixedPosition[1];
		if(mask[2] != 0)  (b->physicalParameters.get())->se3.position[2] = fixedPosition[2];
	}
}

YADE_PLUGIN();
