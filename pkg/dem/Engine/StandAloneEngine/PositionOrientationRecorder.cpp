/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "PositionOrientationRecorder.hpp"
#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>
#include <boost/lexical_cast.hpp>


PositionOrientationRecorder::PositionOrientationRecorder () : DataRecorder()
{
	outputFile = "positionorientation";
	interval = 50;
	saveRgb=false;
}


PositionOrientationRecorder::~PositionOrientationRecorder ()
{

}


void PositionOrientationRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing) {}
}


void PositionOrientationRecorder::registerAttributes()
{
	DataRecorder::registerAttributes();
	REGISTER_ATTRIBUTE(outputFile);
	REGISTER_ATTRIBUTE(interval);
	REGISTER_ATTRIBUTE(saveRgb);
}


void PositionOrientationRecorder::action(Body * body)
{
	MetaBody* ncb = YADE_CAST<MetaBody*>(body);
	if( Omega::instance().getCurrentIteration() % interval == 0 ){
		ostringstream oss;
		oss<<setfill('0')<<outputFile<<"_"<<setw(6)<<Omega::instance().getCurrentIteration();
		cerr<<"Snapshot "<<oss.str()<<endl;
		std::ofstream ofile,rgbFile;
		ofile.open(oss.str().c_str());
		if(saveRgb) rgbFile.open((oss.str()+".rgb").c_str());
		if(!ofile.good()){ cerr<<"Snapshot "<<oss.str()<<" could not be opened for writing (skipping)!"<<endl; return; }
		if(saveRgb && !rgbFile.good()){ cerr<<"Snapshot "<<oss.str()<<" could not be opened for writing (skipping)!"<<endl; return; }
	
		BodyContainer::iterator biEnd = ncb->bodies->end();
		for(BodyContainer::iterator bi    = ncb->bodies->begin(); bi!=biEnd; ++bi){
			const Se3r& se3=(*bi)->physicalParameters->se3;
			const Vector3r& color=(*bi)->geometricalModel->diffuseColor;
			ofile<<se3.position[0]<<" "<<se3.position[1]<<" "<<se3.position[2]<<" "<<se3.orientation[0]<<" "<<se3.orientation[1]<<" "<<se3.orientation[2]<<" "<<se3.orientation[3]<<endl;
			if(saveRgb) rgbFile<<color[0]<<" "<<color[1]<<" "<<color[2]<<endl;
		}
		ofile.close();
		if(saveRgb) rgbFile.close();
	}
}


YADE_PLUGIN();
