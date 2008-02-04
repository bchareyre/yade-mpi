/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ForceRecorder.hpp"
#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/Force.hpp>
#include <boost/lexical_cast.hpp>


ForceRecorder::ForceRecorder () : DataRecorder(), actionForce(new Force)
{
	outputFile = "";
	interval = 50;
	startId = 0;
	endId = 1;
	changed = false;
}


void ForceRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		ofile.open(outputFile.c_str());
	}
}


void ForceRecorder::registerAttributes()
{
	DataRecorder::registerAttributes();
	REGISTER_ATTRIBUTE(outputFile);
	REGISTER_ATTRIBUTE(interval);
	REGISTER_ATTRIBUTE(startId);
	REGISTER_ATTRIBUTE(endId);
}


bool ForceRecorder::isActivated()
{
	return ((Omega::instance().getCurrentIteration() % interval == 0) && (ofile));
}


void ForceRecorder::action(Body * body)
{
	MetaBody * ncb = YADE_CAST<MetaBody*>(body);
	Real x=0, y=0, z=0;
	
	for( int i = startId ; i <= endId ; ++i )
	{
		if(ncb->bodies->exists(i))
		{
			Vector3r force = YADE_CAST<Force*>(ncb->physicalActions->find( i , actionForce->getClassIndex() ) . get() )->force;
		
			x+=force[0];
			y+=force[1];
			z+=force[2];
		}
	}
	//cerr << "record force" << endl;
	ofile << lexical_cast<string>(Omega::instance().getSimulationTime()) << " " 
		<< lexical_cast<string>(x) << " " 
		<< lexical_cast<string>(y) << " " 
		<< lexical_cast<string>(z) << endl;
		

		
}

YADE_PLUGIN();
