/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"Omega.hpp"
#include"yadeExceptions.hpp"
#include"MetaBody.hpp"
#include"TimeStepper.hpp"
#include"ThreadRunner.hpp"
#include"Preferences.hpp"
#include<Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/lib-serialization/IOFormatManager.hpp>
#include<yade/lib-serialization/FormatChecker.hpp>
#include<yade/lib-multimethods/FunctorWrapper.hpp>
#include<yade/lib-multimethods/Indexable.hpp>
#include<cstdlib>
#include<boost/filesystem/operations.hpp>
#include<boost/filesystem/convenience.hpp>
#include<boost/algorithm/string.hpp>

#include<cxxabi.h>

CREATE_LOGGER(Omega);

Omega::Omega(){
	if(getenv("YADE_DEBUG")) cerr<<"Constructing Omega; _must_ be only once, otherwise linking is broken (missing -rdynamic?)\n";
	init(); timeInit();
}

Omega::~Omega(){LOG_INFO("Shuting down; duration "<<(microsec_clock::local_time()-msStartingSimulationTime)/1000<<" s");}

const map<string,DynlibDescriptor>& Omega::getDynlibsDescriptor(){return dynlibs;}

void Omega::incrementCurrentIteration(){ if(rootBody) rootBody->currentIteration++;}
long int Omega::getCurrentIteration(){ return (rootBody?rootBody->currentIteration:-1); }
void Omega::setCurrentIteration(long int i) { if(rootBody) rootBody->currentIteration=i; }

Real Omega::getSimulationTime() { return rootBody?rootBody->simulationTime:-1;};
void Omega::incrementSimulationTime() { if(rootBody) rootBody->simulationTime+=getTimeStep();};

void Omega::setSimulationFileName(const string f){simulationFileName = f;}
string Omega::getSimulationFileName(){return simulationFileName;}

const shared_ptr<MetaBody>& Omega::getRootBody(){return rootBody;}
void Omega::setRootBody(shared_ptr<MetaBody>& rb){ rootBody=rb;}
void Omega::resetRootBody(){ rootBody = shared_ptr<MetaBody>(new MetaBody);}
boost::mutex& Omega::getRootBodyMutex(){return rootBodyMutex;}

ptime Omega::getMsStartingSimulationTime(){return msStartingSimulationTime;}
time_duration Omega::getSimulationPauseDuration(){return simulationPauseDuration;}
Real Omega::getComputationTime(){ return (microsec_clock::local_time()-msStartingSimulationTime-simulationPauseDuration).total_milliseconds()/1e3; }
time_duration Omega::getComputationDuration(){return microsec_clock::local_time()-msStartingSimulationTime-simulationPauseDuration;}


void Omega::reset(){
	//finishSimulationLoop();
	joinSimulationLoop();
	init();
}

void Omega::init(){
	simulationFileName="";
	resetRootBody();
}

void Omega::timeInit(){
	msStartingSimulationTime=microsec_clock::local_time();
	simulationPauseDuration=msStartingSimulationTime-msStartingSimulationTime;
	msStartingPauseTime=msStartingSimulationTime;
}

void Omega::createSimulationLoop(){	simulationLoop=shared_ptr<ThreadRunner>(new ThreadRunner(&simulationFlow_));}
void Omega::finishSimulationLoop(){ LOG_DEBUG(""); if (simulationLoop&&simulationLoop->looping())simulationLoop->stop();}
void Omega::joinSimulationLoop(){ LOG_DEBUG(""); finishSimulationLoop(); if (simulationLoop) simulationLoop=shared_ptr<ThreadRunner>(); }

/* WARNING: single simulation is still run asynchronously; the call will return before the iteration is finished.
 */
void Omega::spawnSingleSimulationLoop(){
	if (simulationLoop){
		msStartingPauseTime = microsec_clock::local_time();
		simulationLoop->spawnSingleAction();
	}
}



void Omega::startSimulationLoop(){
	if (simulationLoop && !simulationLoop->looping()){
		simulationPauseDuration += microsec_clock::local_time()-msStartingPauseTime;
		simulationLoop->start();
	}
}


void Omega::stopSimulationLoop(){
	if (simulationLoop && simulationLoop->looping()){
		msStartingPauseTime = microsec_clock::local_time();
		simulationLoop->stop();
	}
}

bool Omega::isRunning(){ if(simulationLoop) return simulationLoop->looping(); else return false; }

void Omega::buildDynlibDatabase(const vector<string>& dynlibsList){	
	FOREACH(string name, dynlibsList){
		shared_ptr<Factorable> f;
		try {
			f = ClassFactory::instance().createShared(name);
			dynlibs[name].isIndexable    = dynamic_pointer_cast<Indexable>(f);
			dynlibs[name].isFactorable   = dynamic_pointer_cast<Factorable>(f);
			dynlibs[name].isSerializable = dynamic_pointer_cast<Serializable>(f);
			for(int i=0;i<f->getBaseClassNumber();i++)
				dynlibs[name].baseClasses.insert(f->getBaseClassName(i));
		}
		catch (FactoryError& e){
			/* FIXME: this catches all errors! Some of them are not harmful, however:
			 * when a class is not factorable, it is OK to skip it; */	
		}
	}

	map<string,DynlibDescriptor>::iterator dli    = dynlibs.begin();
	map<string,DynlibDescriptor>::iterator dliEnd = dynlibs.end();
	for( ; dli!=dliEnd ; ++dli){
		set<string>::iterator bci    = (*dli).second.baseClasses.begin();
		set<string>::iterator bciEnd = (*dli).second.baseClasses.end();
		for( ; bci!=bciEnd ; ++bci){
			string name = *bci;
			if (name=="MetaEngine1D" || name=="MetaEngine2D") (*dli).second.baseClasses.insert("MetaEngine");
			else if (name=="EngineUnit1D" || name=="EngineUnit2D") (*dli).second.baseClasses.insert("EngineUnit");
			else if (name=="Serializable") (*dli).second.baseClasses.insert("Factorable");
			else if (name!="Factorable" && name!="Indexable") {
				shared_ptr<Factorable> f = ClassFactory::instance().createShared(name);
				for(int i=0;i<f->getBaseClassNumber();i++)
					dynlibs[name].baseClasses.insert(f->getBaseClassName(i));
			}
		}
	}
}


bool Omega::isInheritingFrom(const string& className, const string& baseClassName){
	return (dynlibs[className].baseClasses.find(baseClassName)!=dynlibs[className].baseClasses.end());
}

void Omega::scanPlugins()
{
	FOREACH(string dld,preferences->dynlibDirectories) ClassFactory::instance().addBaseDirectory(dld);
	vector<string> dynlibsList;
	FOREACH(string si, preferences->dynlibDirectories){
		filesystem::path directory(si);
		if(!filesystem::exists(directory)){LOG_ERROR("Nonexistent plugin directory: "<<directory.native_directory_string()<<".");continue; }
		filesystem::directory_iterator di(directory),diEnd;
		FOREACH(filesystem::path pth,std::make_pair(di,diEnd)){
			// node is not a directory and is either regular file or non-dangling symlink; and extension is not ".a"; AND moreover transforming it to library name and back to filename is identity; otherwise the file wouldn't be loaded by the DynLibManager anyway
			if (!filesystem::is_directory(*di) && filesystem::exists(*di) && filesystem::extension(*di)!=".a" &&
				ClassFactory::instance().libNameToSystemName(ClassFactory::instance().systemNameToLibName(filesystem::basename(pth)))==(pth.leaf())){
				filesystem::path name(filesystem::basename(pth));
				// warning: this can produce invalid name (too short).
				// 0-length names are dumped directly
				// names 0<length<4 should fail assertion in DynLibManager::systemNameToLibName
				// the whole loading "logic" should be rewritten from scratch...
				if(name.leaf().length()<1) continue; // filter out 0-length filenames
				if(dynlibsList.size()==0 || ClassFactory::instance().systemNameToLibName(name.leaf())!=dynlibsList.back()) {
					LOG_DEBUG("Added plugin: "<<si<<"/"<<pth.leaf()<<".");
					dynlibsList.push_back(ClassFactory::instance().systemNameToLibName(name.leaf()));
				}
				else LOG_DEBUG("Possible plugin discarded: "<<si<<"/"<<name.leaf()<<".");
			} else LOG_DEBUG("File not considered a plugin: "<<pth.leaf()<<".");
		}
	}

	bool allLoaded = true;
	vector<string> dynlibsClassList; // dynlibsList holds filenames, this holds classes defined inside (may be different if using yadePuginClasses)
	FOREACH(string dll, dynlibsList){
		bool thisLoaded = ClassFactory::instance().load(dll);
		if (!thisLoaded){
			string err=ClassFactory::instance().lastError();
			// HACK
			if(err.find("cannot open shared object file: No such file or directory")!=std::string::npos){
				LOG_INFO("Attempted to load nonexistent file; since this may be due to bad algorithm of filename construction, we pretend everything is OK (original error: `"<<err<<"').");
				thisLoaded=true;
			}
			else if(err.find(": undefined symbol: ")!=std::string::npos){
				size_t pos=err.rfind(":");
				assert(pos!=std::string::npos);
				std::string sym(err,pos+2); //2 removes ": " from the beginning
				int status=0;
				char* demangled_sym=abi::__cxa_demangle(sym.c_str(),0,0,&status);
				LOG_FATAL("Undefined symbol `"<<demangled_sym<<"' ("<<err<<").");
			}
			else LOG_ERROR("Error loading Library `"<<dll<<"': "<<err<<" ."); // leave space to not to confuse c++filt
		}
		else { // no error
			if (ClassFactory::instance().lastPluginClasses().size()==0){ // regular plugin, one class per file
				dynlibsClassList.push_back(dll);
				LOG_DEBUG("Plugin "<<dll<<": loaded default class "<<dll<<".");
			} else {// if plugin defines yadePluginClasses (has multiple classes), insert these into dynLibsList
				vector<string> css=ClassFactory::instance().lastPluginClasses();
				for(size_t i=0; i<css.size();i++) { dynlibsClassList.push_back(css[i]); LOG_DEBUG("Plugin "<<dll<<": loaded explicit class "<<css[i]<<".");  }
			}
		}
		allLoaded &= thisLoaded;
	}
	if(!allLoaded) { LOG_FATAL("Error loading a plugin (see above; run with -v to see more), bailing out."); abort(); }
	buildDynlibDatabase(dynlibsClassList);
}

void Omega::loadSimulationFromStream(std::istream& stream){
	LOG_DEBUG("Loading simulation from stream.");
	resetRootBody();
	timeInit();
	IOFormatManager::loadFromStream("XMLFormatManager",stream,"rootBody",rootBody);
}
void Omega::saveSimulationToStream(std::ostream& stream){
	LOG_DEBUG("Saving simulation to stream.");
	IOFormatManager::saveToStream("XMLFormatManager",stream,"rootBody",rootBody);
}

void Omega::loadSimulation()
{
	if(Omega::instance().getSimulationFileName().size()==0) throw yadeBadFile("Simulation filename to load has zero length");
	if(!filesystem::exists(simulationFileName)) throw yadeBadFile("Simulation file to load doesn't exist");
	
	// FIXME: should stop running simulation!!
	LOG_INFO("Loading file " + simulationFileName);

	// FIXME: stop rendering during loading - may lead to crash
	{
		//boost::mutex::scoped_lock lock1(rootBody->persistentInteractions->drawloopmutex);
		//boost::mutex::scoped_lock lock2(rootBody->transientInteractions->drawloopmutex);
		
		if(algorithm::ends_with(simulationFileName,".xml") || algorithm::ends_with(simulationFileName,".xml.gz") || algorithm::ends_with(simulationFileName,".xml.bz2")){
			resetRootBody();
			IOFormatManager::loadFromFile("XMLFormatManager",simulationFileName,"rootBody",rootBody);
		}
		else if(algorithm::ends_with(simulationFileName,".yade")){
			resetRootBody();
			IOFormatManager::loadFromFile("BINFormatManager",simulationFileName,"rootBody",rootBody);
		}
		else throw (yadeBadFile("Extension of file not recognized."));
	}

	if( rootBody->getClassName() != "MetaBody") throw yadeBadFile("Wrong file format (rootBody is not a MetaBody!) ??");

	timeInit();

	LOG_DEBUG("Simulation loaded");
}



void Omega::saveSimulation(const string name)
{
	if(name.size()==0) throw yadeBadFile("Filename with zero length.");
	LOG_INFO("Saving file " << name);

	if(algorithm::ends_with(name,".xml") || algorithm::ends_with(name,".xml.gz") || algorithm::ends_with(name,".xml.bz2")){
		FormatChecker::format=FormatChecker::XML;
		IOFormatManager::saveToFile("XMLFormatManager",name,"rootBody",rootBody);
	}
	else if(algorithm::ends_with(name,".yade")){
		FormatChecker::format=FormatChecker::BIN;
		IOFormatManager::saveToFile("BINFormatManager",name,"rootBody",rootBody);
	}
	else {
		throw(yadeBadFile(("Filename extension not recognized in `"+name+"'").c_str()));
	}
}



void Omega::setTimeStep(const Real t){	if(rootBody) rootBody->dt=t;}
Real Omega::getTimeStep(){	if(rootBody) return rootBody->dt; else return -1; }
void Omega::skipTimeStepper(bool s){ if(rootBody) rootBody->setTimeSteppersActive(!s);}

bool Omega::timeStepperActive(){
	if(!rootBody) return false;
	FOREACH(const shared_ptr<Engine>& e, rootBody->engines){
		if (isInheritingFrom(e->getClassName(),"TimeStepper")){
			return static_pointer_cast<TimeStepper>(e)->active;
		}
	}
	return false;
}

bool Omega::containTimeStepper(){
	if(!rootBody) return false;
	FOREACH(const shared_ptr<Engine>& e, rootBody->engines){
		if (isInheritingFrom(e->getClassName(),"TimeStepper")) return true;
	}
	return false;
}



