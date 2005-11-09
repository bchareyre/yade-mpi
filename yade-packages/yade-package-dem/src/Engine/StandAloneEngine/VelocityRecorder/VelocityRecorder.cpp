/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "VelocityRecorder.hpp"
#include <yade/yade-package-common/ParticleParameters.hpp>
#include <yade/yade-core/Omega.hpp>
#include <yade/yade-core/MetaBody.hpp>
#include <boost/lexical_cast.hpp>


VelocityRecorder::VelocityRecorder () : DataRecorder()
{
	outputFile = "";
	interval = 50;
	bigBallId = 0;
}


void VelocityRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		ofile.open(outputFile.c_str());
	}
}


void VelocityRecorder::registerAttributes()
{
	DataRecorder::registerAttributes();
	REGISTER_ATTRIBUTE(outputFile);
	REGISTER_ATTRIBUTE(interval);
	REGISTER_ATTRIBUTE(bigBallId);
}


bool VelocityRecorder::isActivated()
{
	return ((Omega::instance().getCurrentIteration() % interval == 0) && (ofile));
}


void VelocityRecorder::action(Body * body)
{
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	Real x=0, y=0, z=0, size=0;//, totalMass=0; FIXME- how many recorders/Actors to make simple stuff?
	
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for(  ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> body = *bi;
		if( body->isDynamic && body->getId() != bigBallId )
		{ 
			size+=1.0;
			ParticleParameters* pp = dynamic_cast<ParticleParameters*>(body->physicalParameters.get());
			x+=pp->velocity[0];
			y+=pp->velocity[1];
			z+=pp->velocity[2];
			
//			totalMass += pp->mass;
		}
	}

	x /= size;
	y /= size;
	z /= size;
//	cerr << totalMass << endl;
	ParticleParameters* bigBall = dynamic_cast<ParticleParameters*>((*bodies)[bigBallId]->physicalParameters.get());
	
	ofile << lexical_cast<string>(Omega::instance().getSimulationTime()) << " " 
		<< lexical_cast<string>(x) << " " 
		<< lexical_cast<string>(y) << " " 
		<< lexical_cast<string>(z) << " "

		<< lexical_cast<string>(bigBall->velocity[0]) << " " // big ball
		<< lexical_cast<string>(bigBall->velocity[1]) << " " 
		<< lexical_cast<string>(bigBall->velocity[2]) << endl;
}


