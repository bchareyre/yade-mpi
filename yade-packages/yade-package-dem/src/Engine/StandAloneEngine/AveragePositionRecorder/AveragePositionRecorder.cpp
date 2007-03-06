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


#include <yade/yade-package-common/RigidBodyParameters.hpp>
#include <yade/yade-core/Omega.hpp>
#include <yade/yade-core/MetaBody.hpp>


#include <boost/lexical_cast.hpp>


AveragePositionRecorder::AveragePositionRecorder () : DataRecorder()
{
	outputFile = "";
	interval = 50;
	bigBallId = 0;
}


void AveragePositionRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		ofile.open(outputFile.c_str());
	}
}


void AveragePositionRecorder::registerAttributes()
{
	DataRecorder::registerAttributes();
	REGISTER_ATTRIBUTE(outputFile);
	REGISTER_ATTRIBUTE(interval);
	REGISTER_ATTRIBUTE(bigBallId);
}


bool AveragePositionRecorder::isActivated()
{
	return ((Omega::instance().getCurrentIteration() % interval == 0) && (ofile));
}


void AveragePositionRecorder::action(Body * body)
{
	MetaBody * ncb = Dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	Real x=0, y=0, z=0, size=0;
	
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for(  ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> body = *bi;
		if( body->isDynamic && body->getId() != bigBallId )
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
		<< lexical_cast<string>(z) << " "

		<< lexical_cast<string>((*bodies)[bigBallId]->physicalParameters->se3.position[0]) << " " // big ball
		<< lexical_cast<string>((*bodies)[bigBallId]->physicalParameters->se3.position[1]) << " " 
		<< lexical_cast<string>((*bodies)[bigBallId]->physicalParameters->se3.position[2]) << endl;
}


