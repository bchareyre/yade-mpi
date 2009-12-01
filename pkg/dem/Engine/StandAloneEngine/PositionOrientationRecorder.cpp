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
#include<yade/core/Omega.hpp>
#include<yade/core/World.hpp>
#include <boost/lexical_cast.hpp>
#include<boost/iostreams/filtering_stream.hpp>
#include<boost/iostreams/filter/bzip2.hpp>
#include<boost/iostreams/device/file.hpp>

CREATE_LOGGER(PositionOrientationRecorder);

PositionOrientationRecorder::PositionOrientationRecorder() {
	outputFile = "positionorientation";
	saveRgb=false;
	iterPeriod=50;
}

void PositionOrientationRecorder::action(World * ncb){
	ostringstream oss;
	oss<<setfill('0')<<outputFile<<"_"<<setw(7)<<Omega::instance().getCurrentIteration();
	string fileBase=oss.str();
	iostreams::filtering_ostream ofile; ofile.push(iostreams::bzip2_compressor()); ofile.push(iostreams::file_sink(fileBase+".bz2"));
	iostreams::filtering_ostream rgbFile;
	if(saveRgb){
		rgbFile.push(iostreams::bzip2_compressor());
		rgbFile.push(iostreams::file_sink(fileBase+".rgb.bz2"));
	}
	if(!ofile.good()){ LOG_ERROR("Snapshot "<<fileBase<<".bz2 could not be opened for writing (skipping)!"); return; }
	if(saveRgb && !rgbFile.good()){ LOG_ERROR("Snapshot "<<fileBase<<".rgb.bz2 could not be opened for writing (skipping)!"); return; }
	LOG_INFO("Snapshot "<<fileBase<<".bz2"<<(saveRgb?" (+rgb)":""));
	BodyContainer::iterator biEnd = ncb->bodies->end();
	for(BodyContainer::iterator bi    = ncb->bodies->begin(); bi!=biEnd; ++bi){
		const Se3r& se3=(*bi)->state->se3;
		ofile<<se3.position[0]<<" "<<se3.position[1]<<" "<<se3.position[2]<<" "<<se3.orientation[0]<<" "<<se3.orientation[1]<<" "<<se3.orientation[2]<<" "<<se3.orientation[3]<<endl;
		if(saveRgb && (*bi)->interactingGeometry) {
			const Vector3r& color=(*bi)->interactingGeometry->diffuseColor;
			rgbFile<<color[0]<<" "<<color[1]<<" "<<color[2]<<endl;
		}
	}
}


YADE_PLUGIN((PositionOrientationRecorder));

