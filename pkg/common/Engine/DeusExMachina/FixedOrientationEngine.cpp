/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "FixedOrientationEngine.hpp"
#include<yade/core/MetaBody.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>

FixedOrientationEngine::FixedOrientationEngine()
{
	fixedOrientation=Quaternionr(1.0,0.0,0.0,0.0);
}

void FixedOrientationEngine::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		std::cerr << "fixedOrientation: " << fixedOrientation.W() << " " << fixedOrientation.X() << " " <<fixedOrientation.Y() << " " <<fixedOrientation.Z() << "\n\n";
	}
}



void FixedOrientationEngine::applyCondition(MetaBody * ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	std::vector<int>::const_iterator ii = subscribedBodies.begin();
	std::vector<int>::const_iterator iiEnd = subscribedBodies.end();

	for(;ii!=iiEnd;++ii)
		if( bodies->exists(*ii) )
			((*bodies)[*ii]->physicalParameters.get())->se3.orientation = fixedOrientation;

//	BodyContainer::iterator bi    = bodies->begin();
//	BodyContainer::iterator biEnd = bodies->end();
//	for( ; bi!=biEnd ; ++bi )
//	{
//		shared_ptr<Body> b = *bi;
//		if(mask[0] != 0)  (b->physicalParameters.get())->se3.position[0] = fixedPosition[0];
//		if(mask[1] != 0)  (b->physicalParameters.get())->se3.position[1] = fixedPosition[1];
//		if(mask[2] != 0)  (b->physicalParameters.get())->se3.position[2] = fixedPosition[2];
//	}
}

YADE_PLUGIN((FixedOrientationEngine));