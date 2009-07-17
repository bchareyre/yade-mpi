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
#include<yade/pkg-common/ParticleParameters.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>
#include <boost/lexical_cast.hpp>


VelocityRecorder::VelocityRecorder () : DataRecorder()
{
	outputFile = "";
	interval = 50;
}


void VelocityRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		ofile.open(outputFile.c_str());
	}
}



bool VelocityRecorder::isActivated(MetaBody*)
{
	return ((Omega::instance().getCurrentIteration() % interval == 0) && (ofile));
}


void VelocityRecorder::action(MetaBody * ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	Real x=0, y=0, z=0, size=0;//, totalMass=0; FIXME- how many recorders/Actors to make simple stuff?
	
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for(  ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> body = *bi;
		if( body->isDynamic )
		{ 
			size+=1.0;
			ParticleParameters* pp = YADE_CAST<ParticleParameters*>(body->physicalParameters.get());
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
	
	ofile << lexical_cast<string>(Omega::instance().getSimulationTime()) << " " 
		<< lexical_cast<string>(x) << " " 
		<< lexical_cast<string>(y) << " " 
		<< lexical_cast<string>(z) << endl;
}


YADE_PLUGIN();
