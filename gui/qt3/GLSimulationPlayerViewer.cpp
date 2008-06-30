/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"GLSimulationPlayerViewer.hpp"
#include<stdlib.h>
#include<unistd.h>
#include<fstream>
#include<string>
#include<boost/lexical_cast.hpp>
#include<boost/filesystem/operations.hpp>
#include<boost/filesystem/convenience.hpp>
#include<boost/iostreams/filtering_stream.hpp>
#include<boost/iostreams/filter/bzip2.hpp>
#include<boost/iostreams/device/file.hpp>
#include<boost/algorithm/string.hpp>

#include<yade/lib-opengl/OpenGLWrapper.hpp>
#include<qspinbox.h>
#include"QtSimulationPlayer.hpp"


#undef DEBUG /* I HATE qt3 for this! ::log4cxx::Level::DEBUG becomes ::log4cxx::Level:: becomes syntax error */
#include<yade/pkg-dem/PositionOrientationRecorder.hpp>
using namespace std;
using namespace boost;

CREATE_LOGGER(GLSimulationPlayerViewer);

GLSimulationPlayerViewer::GLSimulationPlayerViewer(QWidget* parent, char* name): GLViewer(0,shared_ptr<OpenGLRenderingEngine>(new OpenGLRenderingEngine),parent,NULL) {
	setSceneRadius(2);
	showEntireScene();
	resize(720, 576);
	setAnimationPeriod(1);
	saveSnapShots=false;
	frameNumber=0;
	lastCheckPointFrame=0;
}

void GLSimulationPlayerViewer::initializeGL(){ QGLViewer::initializeGL(); renderer->init(); }


void GLSimulationPlayerViewer::animate(){
	long ETA_TTL_ms=10000;
	boost::posix_time::ptime now=microsec_clock::local_time();
	if ((now-lastCheckPointTime).total_milliseconds()>ETA_TTL_ms){
		long elapsedFrames=frameNumber-lastCheckPointFrame;
		long framesToGo=xyzFiles.size()-frameNumber;
		if(elapsedFrames>0 && framesToGo>0){
			long ETA_s=((now-lastCheckPointTime).total_milliseconds()/elapsedFrames)*(framesToGo)/1000;
			simPlayer->pushMessage(lexical_cast<string>(framesToGo)+" frames to go, ETA "+lexical_cast<string>(ETA_s/3600)+":"+lexical_cast<string>((ETA_s%3600)/60)+":"+lexical_cast<string>(ETA_s%60));
		}
		lastCheckPointTime=now;
		lastCheckPointFrame=frameNumber;
	}
	if (!loadPositionOrientationFile()){
		frameNumber=0;
		stopAnimation();
		simPlayer->pushMessage("FINISHED");
	} else {
		if (saveSnapShots) {
			setSnapshotFormat("PNG");
			setSnapshotFileName(outputBaseDirectory+"/"+outputBaseName+".png");
			saveSnapshot(/*automatic*/true,/*overwrite*/ true);
			// mimick qglviewer's algorithm for making snapshot filename
			char num[64]; snprintf(num,64,"%04d",frameNumber);
			simPlayer->pushMessage(lexical_cast<string>(frameNumber)+"/"+lexical_cast<string>(xyzFiles.size())+" -> "+outputBaseDirectory+"/"+outputBaseName+"-"+num+".png");
		}
		frameNumber++;
	}
	updateGL();
}

void GLSimulationPlayerViewer::load(const string& fileName, bool fromFile)
{
	if(fromFile){	
		Omega::instance().setSimulationFileName(fileName);
		Omega::instance().loadSimulation();
	}
	updateGL();
	frameNumber=0;
	setSnapshotCounter(0);

	shared_ptr<Engine> _por=Omega::instance().getRootBody()->engineByName("PositionOrientationRecorder");
	if(_por){
		const shared_ptr<PositionOrientationRecorder> por=dynamic_pointer_cast<PositionOrientationRecorder>(_por);
		assert(por);
		LOG_DEBUG("Got PositionOrientationRecorder");
		size_t dirSep=por->outputFile.rfind("/");
		string path,prefix;
		if(dirSep!=string::npos){
			path=por->outputFile.substr(0,dirSep);
			prefix=por->outputFile.substr(dirSep+1 /* without slash, till the end */);
		} else { prefix=por->outputFile; }
		//if(inputBaseDirectory.empty()){ inputBaseDirectory=path; } if(inputBaseName.empty()){ inputBaseName=prefix; }
		inputBaseDirectory=path; inputBaseName=prefix; 
	} else { simPlayer->pushMessage("No PositionOrientationEngine in simulation file. ABORTED.");  return; }

	simPlayer->pushMessage("Using directory "+inputBaseDirectory+" and basename "+inputBaseName+".");
	LOG_DEBUG("Using directory `"<<inputBaseDirectory<<"' and basename `"<<inputBaseName<<"'.");
	if(!filesystem::exists(filesystem::path(inputBaseDirectory))){ LOG_FATAL("Base xyz directory `"<<inputBaseDirectory<<"' doesn't exist!!"); return;}
	filesystem::directory_iterator dEnd;
	xyzFiles.clear();
	for(filesystem::directory_iterator dIter(inputBaseDirectory); dIter!=dEnd; dIter++){
		if(dIter->leaf().find(inputBaseName)!=0 || filesystem::extension(*dIter)==".rgb" || algorithm::ends_with(dIter->string(),".rgb.bz2") || filesystem::is_directory(*dIter) || !filesystem::exists(*dIter)) continue;
		xyzFiles.push_back(dIter->string());
	}
	int stride=simPlayer->sbStride->value();
	xyzFiles.sort();
	if(stride>1){
		list<string> xyz2;
		long i=stride-1;
		FOREACH(string f,xyzFiles){
			if(i++<stride-1) continue;
			i=0; xyz2.push_back(f);
		}
		simPlayer->pushMessage("Using stride of "+lexical_cast<string>(stride)+", reduced files from "+lexical_cast<string>(xyzFiles.size())+" to "+lexical_cast<string>(xyz2.size()));
		xyzFiles=xyz2;
	}
	simPlayer->pushMessage("Found "+lexical_cast<string>(xyzFiles.size())+" files to process.");
	xyzFilesIter=xyzFiles.begin();
	simPlayer->enableControls(true);
}


void GLSimulationPlayerViewer::doOneStep(){
	if (!loadPositionOrientationFile()){
		frameNumber=0;	stopAnimation();
		simPlayer->pushMessage("End of files, stopped.");
	} else frameNumber++;
	updateGL();
}


void GLSimulationPlayerViewer::reset()
{
	frameNumber=0;
	setSnapshotCounter(0);
	xyzFilesIter=xyzFiles.begin();
	loadPositionOrientationFile();
	frameNumber++;
	updateGL();
}


bool GLSimulationPlayerViewer::loadPositionOrientationFile(){
	if(xyzFilesIter==xyzFiles.end()) return false;
	fileName=*(xyzFilesIter++);
	iostreams::filtering_istream f; if(boost::algorithm::ends_with(fileName,".bz2")) f.push(iostreams::bzip2_decompressor()); f.push(iostreams::file_source(fileName));
	// strip .bz2 from the filename
	if(boost::algorithm::ends_with(fileName,".bz2")) algorithm::replace_last(fileName,".bz2","");
	// append .rgb[.bz2]
	string rgbFileName=fileName+".rgb";
	if(!filesystem::exists(rgbFileName)) rgbFileName+=".bz2";
	iostreams::filtering_istream rgb; if(boost::algorithm::ends_with(rgbFileName,".bz2")) rgb.push(iostreams::bzip2_decompressor()); rgb.push(iostreams::file_source(rgbFileName));
	bool doRgb=rgb.good() && filesystem::exists(rgbFileName); // doesn't indicate that the file doesn't exist ?!
	simPlayer->pushMessage(lexical_cast<string>(frameNumber)+"/"+lexical_cast<string>(xyzFiles.size())+" "+fileName+(doRgb?" (+rgb)":""));
	if(!f.good()){LOG_FATAL("Snapshot file "<<fileName<<" could not be opened for reading (fatal, ending sequence)?!"); return false;}
	LOG_TRACE(fileName);
	size_t nBodies=Omega::instance().getRootBody()->bodies->size();
	for(unsigned long id=0; !f.eof() && !f.fail() && id<nBodies; id++){
		shared_ptr<Body> b=Body::byId(id);
		if(!b){ LOG_ERROR("Body #"<<id<<" doesn't exist (skipped)!"); continue; }
		if(!b->physicalParameters) {LOG_ERROR("Body #"<<id<<" has no physical parameters?! (skipping)"); continue; }
		Se3r& mySe3=b->physicalParameters->se3;
		Vector3r& myColor=b->geometricalModel->diffuseColor;
		f>>mySe3.position[0]>>mySe3.position[1]>>mySe3.position[2]>>mySe3.orientation[0]>>mySe3.orientation[1]>>mySe3.orientation[2]>>mySe3.orientation[3];
		if(doRgb && !rgb.eof() && !rgb.fail()) rgb>>myColor[0]>>myColor[1]>>myColor[2];
	}
	return true;
}
