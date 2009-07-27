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
#include<boost/lexical_cast.hpp>
#include<boost/filesystem/operations.hpp>

CREATE_LOGGER(ForceRecorder);

ForceRecorder::ForceRecorder () : DataRecorder()
{
	outputFile = "";
	interval = 50;
	startId = 0;
	endId = 1;
	changed = false;
	first=true;
}


void ForceRecorder::postProcessAttributes(bool deserializing)
{
	//if(deserializing)
	//	ofile.open(outputFile.c_str());
}


void ForceRecorder::init()
{
	first=false;
	if (filesystem::exists(outputFile))
	{
		int i=0;
		while(filesystem::exists(outputFile+"~"+lexical_cast<string>(i))) i++;
		string newFile=outputFile+"~"+lexical_cast<string>(i);
		filesystem::rename(outputFile,newFile);
		LOG_INFO("Renamed old file "<<outputFile<<" to "<<newFile);
	}
	ofile.open(outputFile.c_str());
}


bool ForceRecorder::isActivated(MetaBody*)
{
	return ((Omega::instance().getCurrentIteration() % interval == 0) && (ofile));
}


void ForceRecorder::action(MetaBody * ncb)
{
	if (first) init();
	ncb->bex.sync();

	Real x=0, y=0, z=0;
	
	for( int i = startId ; i <= endId ; ++i )
	{
		if(ncb->bodies->exists(i))
		{
			Vector3r force=ncb->bex.getForce(i);
		
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

YADE_PLUGIN("ForceRecorder");