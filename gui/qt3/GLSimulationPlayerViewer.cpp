/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GLSimulationPlayerViewer.hpp"
#include <stdlib.h>
#include <fstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#undef DEBUG /* I HATE qt3 for this! ::log4cxx::Level::DEBUG becomes ::log4cxx::Level:: becomes syntax error */
#include<yade/pkg-dem/PositionOrientationRecorder.hpp>




using namespace std;
using namespace boost;

CREATE_LOGGER(GLSimulationPlayerViewer);


GLSimulationPlayerViewer::GLSimulationPlayerViewer(QWidget * parent,char* name) : QGLViewer(parent,name)
{
	shared_ptr<Factorable> tmpRenderer = ClassFactory::instance().createShared("OpenGLRenderingEngine");
	renderer = static_pointer_cast<RenderingEngine>(tmpRenderer);

	setSceneRadius(2);
	showEntireScene();
	resize(720, 576);
	setAnimationPeriod(1);
	saveSnapShots = false;
	frameNumber=0;
}


GLSimulationPlayerViewer::~GLSimulationPlayerViewer(){}


void GLSimulationPlayerViewer::initializeGL()
{
	QGLViewer::initializeGL();
	renderer->init();
}


void GLSimulationPlayerViewer::draw()
{
	if (rootBody)
	{
		//renderer->drawBoundingVolume = false;
		//renderer->drawGeometricalModel = true;
		renderer->render(rootBody);
	}
}


void GLSimulationPlayerViewer::fastDraw(){ if (rootBody){renderer->render(rootBody);} }


void GLSimulationPlayerViewer::animate()
{
	if (!loadPositionOrientationFile()){
		frameNumber=0;
		stopAnimation();
	} else {
		if (saveSnapShots) {
			setSnapshotFormat("PNG");
			setSnapshotFileName(outputBaseDirectory+"/"+outputBaseName+".png");
			saveSnapshot(/*automatic*/true,/*overwrite*/ true);
		}
		frameNumber++;
	}
}


void GLSimulationPlayerViewer::load(const string& fileName)
{
	IOFormatManager::loadFromFile("XMLFormatManager",fileName,"rootBody",rootBody);
	updateGL();
	frameNumber=0;
	setSnapshotCounter(0);

	for(vector<shared_ptr<Engine> >::iterator I=rootBody->engines.begin(); I!=rootBody->engines.end(); ++I){
		LOG_TRACE((*I)->getClassName());
		if((*I)->getClassName()=="PositionOrientationRecorder"){
			const shared_ptr<PositionOrientationRecorder> por=dynamic_pointer_cast<PositionOrientationRecorder>(*I);
			assert(por);
			LOG_DEBUG("Got PositionOrientationRecorder");
			size_t dirSep=por->outputFile.rfind("/");
			string path,prefix;
			if(dirSep!=string::npos){
				path=por->outputFile.substr(0,dirSep);
				prefix=por->outputFile.substr(dirSep+1 /* without slash, till the end */);
			} else { prefix=por->outputFile; }
			if(inputBaseDirectory.empty()){ inputBaseDirectory=path; } if(inputBaseName.empty()){ inputBaseName=prefix; }
		}
	}
	
	LOG_DEBUG("Using directory `"<<inputBaseDirectory<<"' and basename `"<<inputBaseName<<"'.");
	if(!filesystem::exists(filesystem::path(inputBaseDirectory))){ LOG_FATAL("Base xyz directory `"<<inputBaseDirectory<<"' doesn't exist!!"); return;}
	filesystem::directory_iterator dEnd;
	xyzFiles.clear();
	for(filesystem::directory_iterator dIter(inputBaseDirectory); dIter!=dEnd; dIter++){
		if(dIter->leaf().find(inputBaseName)!=0 || filesystem::is_directory(*dIter) || !filesystem::exists(*dIter)) continue;
		xyzFiles.push_back(dIter->string());
		//LOG_TRACE("Added "<<dIter->string());
	}
	xyzFiles.sort();
	xyzFilesIter=xyzFiles.begin();
	//for(list<string>::iterator I=xyzFiles.begin(); I!=xyzFiles.end(); I++){LOG_TRACE(*I);}
}


void GLSimulationPlayerViewer::doOneStep()
{
	if (!loadPositionOrientationFile())
	{
		frameNumber=0;
		stopAnimation();
	}
	else frameNumber++;
	updateGL();
}


void GLSimulationPlayerViewer::reset()
{
	frameNumber = 0;
	setSnapshotCounter(0);
	xyzFilesIter=xyzFiles.begin();
	loadPositionOrientationFile();
	frameNumber++;
	updateGL();
}


bool GLSimulationPlayerViewer::loadPositionOrientationFile()
{
	if(xyzFilesIter==xyzFiles.end()) return false;
	fileName=*(xyzFilesIter++);
	ifstream f(fileName.c_str());
	if(!f.good()){LOG_FATAL("Snapshot file "<<fileName<<" could not be opened for reading (fatal, ending sequence)?!"); return false;}
	LOG_TRACE(fileName);
	for(unsigned long id=0; !f.eof() && !f.fail() && id<=(rootBody->bodies->size()-1); id++){
		shared_ptr<Body> b=(*(rootBody->bodies))[id];
		if(!b){ LOG_ERROR("Body #"<<id<<" doesn't exist (skipped)!"); continue; }
		if(!b->physicalParameters) {LOG_ERROR("Body #"<<id<<" has no physical parameters?! (skipping)"); continue; }
		Se3r& mySe3=b->physicalParameters->se3;
		f>>mySe3.position[0]>>mySe3.position[1]>>mySe3.position[2]>>mySe3.orientation[0]>>mySe3.orientation[1]>>mySe3.orientation[2]>>mySe3.orientation[3];
	}
	f.close();
	return true;
}


