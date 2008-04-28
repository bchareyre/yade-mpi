/*************************************************************************
*  Copyright (C) 2007 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "PositionRecorder.hpp"
#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>
#include <boost/lexical_cast.hpp>


PositionRecorder::PositionRecorder () : DataRecorder()
{
	outputFile = "";
	interval = 50;
	startId = 0;
	endId = 1;
}


PositionRecorder::~PositionRecorder ()
{

}


void PositionRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		ofile.open(outputFile.c_str());
	}
}


void PositionRecorder::registerAttributes()
{
	DataRecorder::registerAttributes();
	REGISTER_ATTRIBUTE(outputFile);
	REGISTER_ATTRIBUTE(interval);
	REGISTER_ATTRIBUTE(startId);
	REGISTER_ATTRIBUTE(endId);
}

bool PositionRecorder::isActivated()
{
	return ((Omega::instance().getCurrentIteration() % interval == 0) && (ofile));
}



void PositionRecorder::action(MetaBody * ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	Vector3r pos(Vector3r::ZERO);

	//std::vector<int>::const_iterator ii = subscribedBodies.begin();
	//std::vector<int>::const_iterator iiEnd = subscribedBodies.end();
	
// 	cerr << "Le cerr dans action de PositionRecorder marche !";
// 	cout << "Le cout aussi !!";

	Real count=0;
	for( unsigned int i = startId ; i <= endId ; ++i )
	//for(;ii!=iiEnd;++ii)
		if( bodies->exists(i) )
		{
			pos+=((*bodies)[i]->physicalParameters.get())->se3.position;
			count+=1.0;
		}


	if(count!=0) pos/=count;

	ofile << lexical_cast<string>(Omega::instance().getSimulationTime()) << " " 
		<< lexical_cast<string>(pos[0]) << " " 
		<< lexical_cast<string>(pos[1]) << " " 
		<< lexical_cast<string>(pos[2]) << endl;
}


