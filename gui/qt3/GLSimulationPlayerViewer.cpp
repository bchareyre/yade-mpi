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
#include<yade/lib-opengl/OpenGLWrapper.hpp>


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
	drawGridXYZ[0]=drawGridXYZ[1]=drawGridXYZ[2]=false;
	

	// cut&paste from GLViewer::notMoving()
	camera()->frame()->setWheelSensitivity(-1.0f);
	setMouseBinding(Qt::LeftButton + Qt::RightButton, CAMERA, ZOOM);
	setMouseBinding(Qt::MidButton, CAMERA, ZOOM);
	setMouseBinding(Qt::LeftButton, CAMERA, ROTATE);
	setMouseBinding(Qt::RightButton, CAMERA, TRANSLATE);
	setWheelBinding(Qt::NoButton, CAMERA, ZOOM);
	setMouseBinding(Qt::SHIFT + Qt::LeftButton, SELECT);
	setMouseBinding(Qt::SHIFT + Qt::LeftButton + Qt::RightButton, FRAME, ZOOM);
	setMouseBinding(Qt::SHIFT + Qt::MidButton, FRAME, TRANSLATE);
	setMouseBinding(Qt::SHIFT + Qt::RightButton, FRAME, ROTATE);
	setWheelBinding(Qt::ShiftButton , FRAME, ZOOM);

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
	// this is to allow manipulation of bodies from python, Omega().bodies etc.
	Omega::instance().setRootBody(rootBody);

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
		if(dIter->leaf().find(inputBaseName)!=0 || filesystem::extension(*dIter)==".rgb" || filesystem::is_directory(*dIter) || !filesystem::exists(*dIter)) continue;
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
	ifstream rgb((fileName+".rgb").c_str());
	bool doRgb=rgb.good();
	if(!f.good()){LOG_FATAL("Snapshot file "<<fileName<<" could not be opened for reading (fatal, ending sequence)?!"); return false;}
	LOG_TRACE(fileName);
	for(unsigned long id=0; !f.eof() && !f.fail() && id<=(rootBody->bodies->size()-1); id++){
		shared_ptr<Body> b=(*(rootBody->bodies))[id];
		if(!b){ LOG_ERROR("Body #"<<id<<" doesn't exist (skipped)!"); continue; }
		if(!b->physicalParameters) {LOG_ERROR("Body #"<<id<<" has no physical parameters?! (skipping)"); continue; }
		Se3r& mySe3=b->physicalParameters->se3;
		Vector3r& myColor=b->geometricalModel->diffuseColor;
		f>>mySe3.position[0]>>mySe3.position[1]>>mySe3.position[2]>>mySe3.orientation[0]>>mySe3.orientation[1]>>mySe3.orientation[2]>>mySe3.orientation[3];
		/* try loading color as well */
		if(doRgb && !rgb.eof() && !rgb.fail()) rgb>>myColor[0]>>myColor[1]>>myColor[2];
	}
	f.close();
	rgb.close();
	return true;
}

// parts copied from GLViewer::keyPressEvent
void GLSimulationPlayerViewer::keyPressEvent(QKeyEvent *e){
	     if(e->key()==Qt::Key_G) drawGridXYZ[2]=!drawGridXYZ[2], updateGL();
	else if(e->key()==Qt::Key_X) drawGridXYZ[0]=!drawGridXYZ[0], updateGL();
	else if(e->key()==Qt::Key_Y) drawGridXYZ[1]=!drawGridXYZ[1], updateGL();
	else if(e->key()==Qt::Key_Z) drawGridXYZ[2]=!drawGridXYZ[2], updateGL();
	else if(e->key()==Qt::Key_T){ if (camera()->type() == qglviewer::Camera::ORTHOGRAPHIC) camera()->setType(qglviewer::Camera::PERSPECTIVE); else camera()->setType(qglviewer::Camera::ORTHOGRAPHIC);}
	else if(e->key()==Qt::Key_O ) camera()->setFieldOfView(camera()->fieldOfView()*0.9), updateGL();
	else if(e->key()==Qt::Key_P ) camera()->setFieldOfView(camera()->fieldOfView()*1.1), updateGL();
	else if(e->key()!=Qt::Key_Escape && e->key()!=Qt::Key_Space) QGLViewer::keyPressEvent(e);
}

// copied from GLViewer::postDraw
void GLSimulationPlayerViewer::postDraw(){
	if(drawGridXYZ[0]||drawGridXYZ[1]||drawGridXYZ[2])
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		qglColor(foregroundColor());
		glDisable(GL_LIGHTING);
		glLineWidth(0.1);
		glBegin(GL_LINES);
			Real diameter=QGLViewer::camera()->sceneRadius()*2;
			qglviewer::Vec center=QGLViewer::camera()->sceneCenter();
			Real gridStep=pow(10,(floor(log10(diameter)-.5)));
			int nLines=2*2*((int)(diameter/gridStep))/2+1; // odd number
			int lineNoExt=(nLines-1)/2;
			for(int planeAxis=0; planeAxis<3; planeAxis++){
				if(!drawGridXYZ[planeAxis]) continue;
				int otherAxes[2]={(planeAxis+1)%3,(planeAxis+2)%3};
				Vector3r color(.3,.3,.3); color[planeAxis]=.6;
				glColor3v(color);
				for(int lineAxisIdx=0; lineAxisIdx<2; lineAxisIdx++){
					int lineAxis=otherAxes[lineAxisIdx];
					int linePerp=otherAxes[(lineAxisIdx+1)%2];
					for(int lineNo=-lineNoExt; lineNo<=lineNoExt; lineNo++){
						Vector3r from,to;
						from[planeAxis]=to[planeAxis]=0;
						from[linePerp]=to[linePerp]=center[linePerp]+lineNo*gridStep;
						from[lineAxis]=center[lineAxis]-lineNoExt*gridStep; to[lineAxis]=center[lineAxis]+lineNoExt*gridStep;
						glVertex3v(from); glVertex3v(to);
					}
				}
			}
		glEnd();
		glPopAttrib();
		glPopMatrix();
	}
	QGLViewer::postDraw();
}
