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
#include<sstream>
#include<iomanip>
#include<boost/lexical_cast.hpp>
#include<boost/filesystem/operations.hpp>
#include<boost/filesystem/convenience.hpp>
#include<boost/iostreams/filtering_stream.hpp>
#include<boost/iostreams/filter/bzip2.hpp>
#include<boost/iostreams/device/file.hpp>
#include<boost/algorithm/string.hpp>
#include<boost/date_time/posix_time/posix_time.hpp>

#include<yade/lib-opengl/OpenGLWrapper.hpp>
#include<yade/pkg-common/FilterEngine.hpp>
#include<qspinbox.h>
#include"QtSimulationPlayer.hpp"


#undef DEBUG /* I HATE qt3 for this! ::log4cxx::Level::DEBUG becomes ::log4cxx::Level:: becomes syntax error */
#include<yade/pkg-dem/PositionOrientationRecorder.hpp>
using namespace std;
using namespace boost;

CREATE_LOGGER(GLSimulationPlayerViewer);

void GLSimulationPlayerViewer::closeEvent(QCloseEvent* ev){
	simPlayer->close();
}

void GLSimulationPlayerViewer::keyPressEvent(QKeyEvent* e){
	if(e->key()==Qt::Key_H && (e->state() & AltButton)){ if(simPlayer->isHidden()) simPlayer->show(); else simPlayer->hide(); }
	else if(e->key()==Qt::Key_Backspace && !trigger) { trigger=true; }
	else GLViewer::keyPressEvent(e);
}

GLSimulationPlayerViewer::GLSimulationPlayerViewer(QWidget* parent,shared_ptr<OpenGLRenderingEngine> renderer): GLViewer(/*special value meaning player*/ -1,renderer,parent,NULL) {
	setSceneRadius(2);
	showEntireScene();
	resize(550, 550);
	setAnimationPeriod(0); // as fast as possible
	saveSnapShots=false;
	frameNumber=0;
	stride=1;
	lastCheckPointFrame=0;
	LOG_INFO("Use Alt-H to hide/show the player controller")
}

void GLSimulationPlayerViewer::initializeGL(){ QGLViewer::initializeGL(); renderer->init(); }


void GLSimulationPlayerViewer::animate(){
	long ETA_TTL_ms=10000;
	boost::posix_time::ptime now=microsec_clock::local_time();
	// display the ETA
	if ((now-lastCheckPointTime).total_milliseconds()>ETA_TTL_ms){
		long elapsedFrames=frameNumber-lastCheckPointFrame;
		long framesToGo=xyzNames.size()-frameNumber;
		if(elapsedFrames>0 && framesToGo>0){
			long ETA_s=((now-lastCheckPointTime).total_milliseconds()/elapsedFrames)*(framesToGo)/1000;
			ostringstream oss;
			oss<<framesToGo<<" frames to go, ETA "<<ETA_s/3600<<":"<<setw(2)<<setfill('0')<<((ETA_s%3600)/60)<<":"<<setw(2)<<setfill('0')<<ETA_s%60;
			simPlayer->pushMessage(oss.str());
			// simPlayer->pushMessage(lexical_cast<string>(framesToGo)+" frames to go, ETA "+lexical_cast<string>(ETA_s/3600)+":"+lexical_cast<string>((ETA_s%3600)/60)+":"+lexical_cast<string>(ETA_s%60));
		}
		lastCheckPointTime=now;
		lastCheckPointFrame=frameNumber;
	}
	// try loading next state
	if (!loadNextRecordedData()){
		frameNumber=0;
		stopAnimation();
		simPlayer->pushMessage("FINISHED");
	} else {
		if (saveSnapShots) {
			setSnapshotFormat("PNG");
			setSnapshotFileName(snapshotsBase+".png");
			saveSnapshot(/*automatic*/true,/*overwrite*/ true);
			// mimick qglviewer's algorithm for making snapshot filename
			char num[64]; snprintf(num,64,"%.04d",frameNumber);
			snapshots.push_back(snapshotsBase+"-"+num+".png");
			simPlayer->pushMessage(lexical_cast<string>(frameNumber)+"/"+lexical_cast<string>(xyzNames.size())+" -> "+*snapshots.rbegin());
			//+snapshotsBase+"-"+num+".png");
		}
		frameNumber++;
	}
	updateGL();
}

void GLSimulationPlayerViewer::load(const string& fileName, bool fromFile)
{
	frameNumber=0;
	setSnapshotCounter(0);
	simPlayer->enableControls(false);
	useSQLite=!(algorithm::ends_with(fileName,".xml.bz2")||algorithm::ends_with(fileName,".xml.gz")||algorithm::ends_with(fileName,".xml"));
	xyzNames.clear();
	filters.clear();

	if(!useSQLite){
		if(fromFile){	
			Omega::instance().setSimulationFileName(fileName);
			Omega::instance().loadSimulation();
		}
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
		if(!filesystem::exists(filesystem::path(inputBaseDirectory))) 
		{ 
			if (inputBaseDirectory[0]!='/')  // may be relative path?
			{
					size_t dirSep=fileName.rfind("/");
					string path;
					if(dirSep!=string::npos) path=fileName.substr(0,dirSep);
					inputBaseDirectory=path+"/"+inputBaseDirectory;
			}
			if(!filesystem::exists(filesystem::path(inputBaseDirectory)))
			{
				LOG_FATAL("Base xyz directory `"<<inputBaseDirectory<<"' doesn't exist!!"); 
				xyzNames.clear();
				return;
			}
		}
	
		filesystem::directory_iterator dEnd;
		xyzNames.clear();
		for(filesystem::directory_iterator dIter(inputBaseDirectory); dIter!=dEnd; dIter++){
			if(dIter->leaf().find(inputBaseName)!=0 || filesystem::extension(*dIter)==".rgb" || algorithm::ends_with(dIter->string(),".rgb.bz2") || filesystem::is_directory(*dIter) || !filesystem::exists(*dIter)) continue;
			xyzNames.push_back(dIter->string());
		}
		xyzNames.sort();
	} else { /* load from sqlite database */
		LOG_INFO("Opening sqlite database `"<<fileName<<"'");
		con=shared_ptr<sqlite3x::sqlite3_connection>(new sqlite3x::sqlite3_connection(fileName));
		if(0==con->executeint("select count(*) from sqlite_master where name='meta';")){ LOG_ERROR("Database doesn't have the 'meta' table."); return; }
		if(0==con->executeint("select count(*) from sqlite_master where name='records';")){ LOG_ERROR("Database doesn't have the 'records' table."); return; }
		simPlayer->pushMessage("Database OK, loading simulation...");
		// load simulation
		string xml=con->executestring("select simulationXML from 'meta';");
		simPlayer->pushMessage("Simulation loaded.");
		istringstream xmlStream(xml); Omega::instance().loadSimulationFromStream(xmlStream);
		{
			sqlite3x::sqlite3_command cmd(*con,"select bodyTable from 'records' ORDER BY iter;");
			sqlite3x::sqlite3_cursor reader=cmd.executecursor();
			assert(reader.colcount()==1);
			while(reader.step()){
				xyzNames.push_back(reader.getstring(0)); // first column
				LOG_DEBUG("Added table "<<reader.getstring(0));
			}
		}
	}
	/* Filters */
	FOREACH(shared_ptr<Engine> e, Omega::instance().getRootBody()->engines){
		shared_ptr<FilterEngine> fe=dynamic_pointer_cast<FilterEngine>(e);
		if(fe){
			filters.push_back(fe);
			simPlayer->pushMessage("Find filter: "+e->getClassName());
		}
	}
	/* strided access is common for both db and file access */
	if(stride>1){
		list<string> xyz2;
		long i=stride-1;
		FOREACH(string f,xyzNames){
			if(i++<stride-1) continue;
			i=0; xyz2.push_back(f);
		}
		simPlayer->pushMessage("Using stride of "+lexical_cast<string>(stride)+", reduced states from "+lexical_cast<string>(xyzNames.size())+" to "+lexical_cast<string>(xyz2.size()));
		xyzNames=xyz2;
	}
	simPlayer->pushMessage("Found "+lexical_cast<string>(xyzNames.size())+" states to process.");
	xyzNamesIter=xyzNames.begin();
	simPlayer->enableControls(true);
	updateGL();
}

void GLSimulationPlayerViewer::doOneStep(){
	if (!loadNextRecordedData()){
		frameNumber=0;	stopAnimation();
		simPlayer->pushMessage("End of files, stopped.");
	} else frameNumber++;
	updateGL();
}

void GLSimulationPlayerViewer::reset()
{
	frameNumber=0;
	setSnapshotCounter(0);
	xyzNamesIter=xyzNames.begin();
	loadNextRecordedData();
	frameNumber++;
	updateGL();
}


bool GLSimulationPlayerViewer::loadNextRecordedData(){
	if(xyzNamesIter==xyzNames.end()) return false;
	if(!useSQLite){
		fileName=*(xyzNamesIter++);
		iostreams::filtering_istream f; if(boost::algorithm::ends_with(fileName,".bz2")) f.push(iostreams::bzip2_decompressor()); f.push(iostreams::file_source(fileName));
		// strip .bz2 from the filename
		if(boost::algorithm::ends_with(fileName,".bz2")) algorithm::replace_last(fileName,".bz2","");
		// append .rgb[.bz2]
		string rgbFileName=fileName+".rgb";
		if(!filesystem::exists(rgbFileName)) rgbFileName+=".bz2";
		iostreams::filtering_istream rgb; if(boost::algorithm::ends_with(rgbFileName,".bz2")) rgb.push(iostreams::bzip2_decompressor()); rgb.push(iostreams::file_source(rgbFileName));
		bool doRgb=rgb.good() && filesystem::exists(rgbFileName); // doesn't indicate that the file doesn't exist ?!
		simPlayer->pushMessage(lexical_cast<string>(frameNumber)+"/"+lexical_cast<string>(xyzNames.size())+" "+fileName+(doRgb?" (+rgb)":""));
		if(!f.good()){LOG_FATAL("Snapshot file "<<fileName<<" could not be opened for reading (fatal, ending sequence)?!"); return false;}
		LOG_TRACE(fileName);
		size_t nBodies=Omega::instance().getRootBody()->bodies->size();
		for(unsigned long id=0; !f.eof() && !f.fail() && id<nBodies; id++){
			shared_ptr<Body> b=Body::byId(id);
			if(!b){ LOG_ERROR("Body #"<<id<<" doesn't exist (skipped)!"); continue; }
			if(!b->physicalParameters) {LOG_ERROR("Body #"<<id<<" has no physical parameters?! (skipping)"); continue; }
			Se3r& mySe3=b->physicalParameters->se3;
			f>>mySe3.position[0]>>mySe3.position[1]>>mySe3.position[2]>>mySe3.orientation[0]>>mySe3.orientation[1]>>mySe3.orientation[2]>>mySe3.orientation[3];
			if(doRgb && !rgb.eof() && !rgb.fail()) {
				if(!b->geometricalModel) {LOG_ERROR("Body #"<<id<<" has no geometrical model?! (skipping)"); continue; }
				Vector3r& myColor=b->geometricalModel->diffuseColor;
				rgb>>myColor[0]>>myColor[1]>>myColor[2];
			}
		}
		Omega::instance().setCurrentIteration(atoi(fileName.substr(fileName.rfind('_')+1).c_str()));
	} else {
		string tableName=*(xyzNamesIter++);
		simPlayer->pushMessage(lexical_cast<string>(frameNumber)+"/"+lexical_cast<string>(xyzNames.size())+" "+tableName);
		sqlite3x::sqlite3_command cmd(*con,"select * from '"+tableName+"';");
		sqlite3x::sqlite3_cursor reader=cmd.executecursor();
		int colcount=reader.colcount();
		int col_id=-1,col_se3_x=-1,col_se3_y=-1,col_se3_z=-1,col_se3_ori0=-1,col_se3_ori1=-1,col_se3_ori2=-1,col_se3_ori3=-1,col_rgb_r=-1, col_rgb_g=-1,col_rgb_b=-1;
		for(int i=0;i<colcount;i++){
			string col=reader.getcolname(i);
			#define ASSIGN_COL(colname) if(col==#colname) col_##colname=i;
			ASSIGN_COL(id) else ASSIGN_COL(se3_x) else ASSIGN_COL(se3_y) else ASSIGN_COL(se3_z) else ASSIGN_COL(se3_ori0) else ASSIGN_COL(se3_ori1) else ASSIGN_COL(se3_ori2) else ASSIGN_COL(se3_ori3) else ASSIGN_COL(rgb_r) else ASSIGN_COL(rgb_g) else ASSIGN_COL(rgb_b) else LOG_ERROR("Unhandled column name: '"<<col<<"'");
		}
		assert(col_id>=0);
		while(reader.step()){
			const shared_ptr<Body>& b=Body::byId(reader.getint(col_id));
			Se3r& se3=b->physicalParameters->se3;
			if(col_se3_x>=0) se3.position[0]=reader.getdouble(col_se3_x);
			if(col_se3_y>=0) se3.position[1]=reader.getdouble(col_se3_y);
			if(col_se3_z>=0) se3.position[2]=reader.getdouble(col_se3_z);
			if(col_se3_ori0>=0) se3.orientation[0]=reader.getdouble(col_se3_ori0);
			if(col_se3_ori1>=0) se3.orientation[1]=reader.getdouble(col_se3_ori1);
			if(col_se3_ori2>=0) se3.orientation[2]=reader.getdouble(col_se3_ori2);
			if(col_se3_ori3>=0) se3.orientation[3]=reader.getdouble(col_se3_ori3);
			se3.orientation.Normalize();
			if(col_rgb_r>=0) b->geometricalModel->diffuseColor[0]=reader.getdouble(col_rgb_r);
			if(col_rgb_g>=0) b->geometricalModel->diffuseColor[1]=reader.getdouble(col_rgb_g);
			if(col_rgb_b>=0) b->geometricalModel->diffuseColor[2]=reader.getdouble(col_rgb_b);
		}
		Omega::instance().setCurrentIteration(con->executeint("select iter from 'records' where bodyTable='"+tableName+"';"));
		Omega::instance().getRootBody()->simulationTime=con->executedouble("select virtTime from 'records' where bodyTable='"+tableName+"';");
		realTime=con->executedouble("select realTime from 'records' where bodyTable='"+tableName+"';");
		wallClock=con->executedouble("select wallClock from 'records' where bodyTable='"+tableName+"';");
	}
	FOREACH(const shared_ptr<FilterEngine>& e, filters) { if(e->isActivated()) e->action(Omega::instance().getRootBody().get()); }
	return true;
}

string GLSimulationPlayerViewer::getRealTimeString(){
	ostringstream oss;
	posix_time::time_duration rt(0,0,(long)realTime,0);
	oss<<posix_time::to_simple_string(rt)<<"."<<(long)(realTime*10)%10;
	posix_time::ptime when=posix_time::from_time_t(wallClock);
	oss<<" ("<<posix_time::to_simple_string(when)<<")";
	return oss.str();
}

void GLSimulationPlayerViewer::bodyWire(bool wire)
{
//	FOREACH(shared_ptr<Body> b, *Omega::instance().getRootBody()->bodies)
//	{b->geometricalModel->wire=wire;}
	renderer->Body_wire=wire;
}
