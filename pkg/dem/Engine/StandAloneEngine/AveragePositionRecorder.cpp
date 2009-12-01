/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "AveragePositionRecorder.hpp"


#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/World.hpp>


#include <boost/lexical_cast.hpp>


AveragePositionRecorder::AveragePositionRecorder () : DataRecorder()
{
	outputFile = "";
	interval = 50;
}


void AveragePositionRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		ofile.open(outputFile.c_str());
	}
}



bool AveragePositionRecorder::isActivated(World*)
{
	return ((Omega::instance().getCurrentIteration() % interval == 0) && (ofile));
}


void AveragePositionRecorder::action(World * ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	Real x=0, y=0, z=0, size=0;
	
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for(  ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> body = *bi;
		if( body->isDynamic )
		{ 
			size+=1.0;
			x+=body->physicalParameters->se3.position[0];
			y+=body->physicalParameters->se3.position[1];
			z+=body->physicalParameters->se3.position[2];
		}
	}

	x /= size;
	y /= size;
	z /= size;
	
	ofile << lexical_cast<string>(Omega::instance().getSimulationTime()) << " " 
		<< lexical_cast<string>(x) << " " 
		<< lexical_cast<string>(y) << " " 
		<< lexical_cast<string>(z) << endl;
}


YADE_PLUGIN((AveragePositionRecorder));

YADE_REQUIRE_FEATURE(PHYSPAR);

