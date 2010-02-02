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
#include"Scene.hpp"
#include"TimeStepper.hpp"
#include"ThreadRunner.hpp"
#include<Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/lib-serialization/IOFormatManager.hpp>
#include<yade/lib-serialization/FormatChecker.hpp>
#include<yade/lib-multimethods/FunctorWrapper.hpp>
#include<yade/lib-multimethods/Indexable.hpp>
#include<cstdlib>
#include<boost/filesystem/operations.hpp>
#include<boost/filesystem/convenience.hpp>
#include<boost/filesystem/exception.hpp>
#include<boost/algorithm/string.hpp>
#include<boost/thread/mutex.hpp>
#include<boost/version.hpp>
#include<boost/python.hpp>

#include<cxxabi.h>

#if BOOST_VERSION<103500
class RenderMutexLock: public boost::try_mutex::scoped_try_lock{
	public:
	RenderMutexLock(): boost::try_mutex::scoped_try_lock(Omega::instance().renderMutex,true){/*cerr<<"Lock renderMutex"<<endl;*/}
	~RenderMutexLock(){/* cerr<<"Unlock renderMutex"<<endl; */}
};
#else
class RenderMutexLock: public boost::mutex::scoped_lock{
	public:
	RenderMutexLock(): boost::mutex::scoped_lock(Omega::instance().renderMutex){/* cerr<<"Lock renderMutex"<<endl; */}
	~RenderMutexLock(){/* cerr<<"Unlock renderMutex"<<endl;*/ }
};
#endif

CREATE_LOGGER(Omega);
SINGLETON_SELF(Omega);

// Omega::~Omega(){LOG_INFO("Shuting down; duration "<<(microsec_clock::local_time()-msStartingSimulationTime)/1000<<" s");}

const map<string,DynlibDescriptor>& Omega::getDynlibsDescriptor(){return dynlibs;}

long int Omega::getCurrentIteration(){ return (scene?scene->currentIteration:-1); }
void Omega::setCurrentIteration(long int i) { if(scene) scene->currentIteration=i; }

Real Omega::getSimulationTime() { return scene?scene->simulationTime:-1;};

void Omega::setSimulationFileName(const string f){simulationFileName = f;}
string Omega::getSimulationFileName(){return simulationFileName;}

const shared_ptr<Scene>& Omega::getScene(){return scene;}
void Omega::setScene(shared_ptr<Scene>& rb){ RenderMutexLock lock; scene=rb;}
void Omega::resetScene(){ RenderMutexLock lock; scene = shared_ptr<Scene>(new Scene);}

ptime Omega::getMsStartingSimulationTime(){return msStartingSimulationTime;}
time_duration Omega::getSimulationPauseDuration(){return simulationPauseDuration;}
Real Omega::getComputationTime(){ return (microsec_clock::local_time()-msStartingSimulationTime-simulationPauseDuration).total_milliseconds()/1e3; }
time_duration Omega::getComputationDuration(){return microsec_clock::local_time()-msStartingSimulationTime-simulationPauseDuration;}


void Omega::initTemps(){
	char dirTemplate[]="/tmp/yade-XXXXXX";
	tmpFileDir=mkdtemp(dirTemplate);
	tmpFileCounter=0;
}

void Omega::cleanupTemps(){
	filesystem::path tmpPath(tmpFileDir);
	filesystem::remove_all(tmpPath);
}

std::string Omega::tmpFilename(){
	if(tmpFileDir.empty()) throw runtime_error("tmpFileDir empty; Omega::initTemps not yet called()?");
	boost::mutex::scoped_lock lock(tmpFileCounterMutex);
	return tmpFileDir+"/tmp-"+lexical_cast<string>(tmpFileCounter++);
}

void Omega::reset(){
	//finishSimulationLoop();
	joinSimulationLoop();
	init();
}

void Omega::init(){
	simulationFileName="";
	resetScene();
	sceneAnother=shared_ptr<Scene>(new Scene);
	timeInit();
	createSimulationLoop();
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
	if(!simulationLoop){ LOG_ERROR("No Omega::simulationLoop? Creating one (please report bug)."); createSimulationLoop(); }
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
	LOG_DEBUG("called with "<<dynlibsList.size()<<" plugins.");
	boost::python::object wrapperScope=boost::python::import("yade.wrapper");
	std::list<string> pythonables;
	FOREACH(string name, dynlibsList){
		shared_ptr<Factorable> f;
		try {
			LOG_DEBUG("Factoring plugin "<<name);
			f = ClassFactory::instance().createShared(name);
			dynlibs[name].isIndexable    = dynamic_pointer_cast<Indexable>(f);
			dynlibs[name].isFactorable   = dynamic_pointer_cast<Factorable>(f);
			dynlibs[name].isSerializable = dynamic_pointer_cast<Serializable>(f);
			for(int i=0;i<f->getBaseClassNumber();i++){
				dynlibs[name].baseClasses.insert(f->getBaseClassName(i));
			}
			if(dynlibs[name].isSerializable) pythonables.push_back(name);
		}
		catch (FactoryError& e){
			/* FIXME: this catches all errors! Some of them are not harmful, however:
			 * when a class is not factorable, it is OK to skip it; */	
		}
	}
	// handle Serializable specially
	//Serializable().pyRegisterClass(wrapperScope);
	/* python classes must be registered such that base classes come before derived ones;
	for now, just loop until we succeed; proper solution will be to build graphs of classes
	and traverse it from the top. It will be done once all classes are pythonable. */
	for(int i=0; i<100 && pythonables.size()>0; i++){
		if(getenv("YADE_DEBUG")) cerr<<endl<<"[[[ Round "<<i<<" ]]]: ";
		std::list<string> done;
		for(std::list<string>::iterator I=pythonables.begin(); I!=pythonables.end(); ){
			shared_ptr<Serializable> s=static_pointer_cast<Serializable>(ClassFactory::instance().createShared(*I));
			try{
				if(getenv("YADE_DEBUG")) cerr<<"{{"<<*I<<"}}";
				s->pyRegisterClass(wrapperScope);
				std::list<string>::iterator prev=I++;
				pythonables.erase(prev);
			} catch (...){
				if(getenv("YADE_DEBUG")) cerr<<"["<<*I<<"]";
				//boost::python::handle_exception();
				I++;
			}
		}
	}

	map<string,DynlibDescriptor>::iterator dli    = dynlibs.begin();
	map<string,DynlibDescriptor>::iterator dliEnd = dynlibs.end();
	for( ; dli!=dliEnd ; ++dli){
		set<string>::iterator bci    = (*dli).second.baseClasses.begin();
		set<string>::iterator bciEnd = (*dli).second.baseClasses.end();
		for( ; bci!=bciEnd ; ++bci){
			string name = *bci;
			if (name=="Dispatcher1D" || name=="Dispatcher2D") (*dli).second.baseClasses.insert("Dispatcher");
			else if (name=="Functor1D" || name=="Functor2D") (*dli).second.baseClasses.insert("Functor");
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

bool Omega::isInheritingFrom_recursive(const string& className, const string& baseClassName){
	if (dynlibs[className].baseClasses.find(baseClassName)!=dynlibs[className].baseClasses.end()) return true;
	FOREACH(const string& parent,dynlibs[className].baseClasses){
		if(isInheritingFrom_recursive(parent,baseClassName)) return true;
	}
	return false;
}

void Omega::loadPlugins(vector<string> pluginFiles){
	FOREACH(const string& plugin, pluginFiles){
		LOG_DEBUG("Loading plugin "<<plugin);
		if(!ClassFactory::instance().load(plugin)){
			string err=ClassFactory::instance().lastError();
			if(err.find(": undefined symbol: ")!=std::string::npos){
				size_t pos=err.rfind(":");	assert(pos!=std::string::npos);
				std::string sym(err,pos+2); //2 removes ": " from the beginning
				int status=0; char* demangled_sym=abi::__cxa_demangle(sym.c_str(),0,0,&status);
				LOG_FATAL(plugin<<": undefined symbol `"<<demangled_sym<<"'"); LOG_FATAL(plugin<<": "<<err); LOG_FATAL("Bailing out.");
			}
			else {
				LOG_FATAL(plugin<<": "<<err<<" ."); /* leave space to not to confuse c++filt */ LOG_FATAL("Bailing out.");
			}
			abort();
		}
	}
	list<string>& plugins(ClassFactory::instance().pluginClasses);
	plugins.sort(); plugins.unique();
	buildDynlibDatabase(vector<string>(plugins.begin(),plugins.end()));
}

void Omega::scanPlugins(vector<string> baseDirs){
	vector<string> pluginFiles;
	FOREACH(const string& baseDir, baseDirs){
		// silently skip non-existent plugin directories
		if(!filesystem::exists(baseDir)) continue;
		try{
			filesystem::recursive_directory_iterator Iend;
			for(filesystem::recursive_directory_iterator I(baseDir); I!=Iend; ++I){ 
				filesystem::path pth=I->path();
				if(filesystem::is_directory(pth) || !algorithm::starts_with(pth.leaf(),"lib") || !algorithm::ends_with(pth.leaf(),".so")) { LOG_DEBUG("File not considered a plugin: "<<pth.leaf()<<"."); continue; }
				filesystem::path name(filesystem::basename(pth));
				if(name.leaf().length()<1) continue; // filter out 0-length filenames
				LOG_DEBUG("Will load plugin "<<pth.leaf());
				pluginFiles.push_back(pth.string());
			}
		} catch(filesystem::basic_filesystem_error<filesystem::path>& e) {
			LOG_FATAL("Error from recursive plugin directory scan (unreadable directory?): "<<e.what());
			throw;
		}
	}
	loadPlugins(pluginFiles);
}

void Omega::loadSimulationFromStream(std::istream& stream){
	LOG_DEBUG("Loading simulation from stream.");
	resetScene();
	RenderMutexLock lock;
	IOFormatManager::loadFromStream("XMLFormatManager",stream,"scene",scene);
}

void Omega::saveSimulationToStream(std::ostream& stream){
	LOG_DEBUG("Saving simulation to stream.");
	IOFormatManager::saveToStream("XMLFormatManager",stream,"scene",scene);
}

void Omega::loadSimulation(){

	if(Omega::instance().getSimulationFileName().size()==0) throw runtime_error("Empty simulation filename to load.");
	if(!filesystem::exists(simulationFileName) && !algorithm::starts_with(simulationFileName,":memory")) throw runtime_error("Simulation file to load doesn't exist: "+simulationFileName);
	
	LOG_INFO("Loading file " + simulationFileName);
	{
		if(algorithm::ends_with(simulationFileName,".xml") || algorithm::ends_with(simulationFileName,".xml.gz") || algorithm::ends_with(simulationFileName,".xml.bz2")){
			joinSimulationLoop(); // stop current simulation if running
			resetScene();
			RenderMutexLock lock; IOFormatManager::loadFromFile("XMLFormatManager",simulationFileName,"scene",scene);
		}
		else if(algorithm::ends_with(simulationFileName,".yade")){
			joinSimulationLoop();
			resetScene();
			RenderMutexLock lock; IOFormatManager::loadFromFile("BINFormatManager",simulationFileName,"scene",scene);
		}
		else if(algorithm::starts_with(simulationFileName,":memory:")){
			if(memSavedSimulations.count(simulationFileName)==0) throw runtime_error("Cannot load nonexistent memory-saved simulation "+simulationFileName);
			istringstream iss(memSavedSimulations[simulationFileName]);
			joinSimulationLoop();
			resetScene();
			RenderMutexLock lock; IOFormatManager::loadFromStream("XMLFormatManager",iss,"scene",scene);
		}
		else throw runtime_error("Extension of file to load not recognized "+simulationFileName);
	}

	if( scene->getClassName() != "Scene") throw runtime_error("Wrong file format (scene is not a Scene!?) in "+simulationFileName);

	timeInit();

	LOG_DEBUG("Simulation loaded");
}



void Omega::saveSimulation(const string name)
{
	if(name.size()==0) throw runtime_error("Name of file to save has zero length.");
	LOG_INFO("Saving file " << name);

	if(algorithm::ends_with(name,".xml") || algorithm::ends_with(name,".xml.bz2")){
		FormatChecker::format=FormatChecker::XML;
		IOFormatManager::saveToFile("XMLFormatManager",name,"scene",scene);
	}
	else if(algorithm::ends_with(name,".yade")){
		FormatChecker::format=FormatChecker::BIN;
		IOFormatManager::saveToFile("BINFormatManager",name,"scene",scene);
	}
	else if(algorithm::starts_with(name,":memory:")){
		if(memSavedSimulations.count(simulationFileName)>0) LOG_INFO("Overwriting in-memory saved simulation "<<name);
		ostringstream oss;
		IOFormatManager::saveToStream("XMLFormatManager",oss,"scene",scene);
		memSavedSimulations[name]=oss.str();
	}
	else {
		throw runtime_error("Filename extension not recognized in `"+name+"'");
	}
}



void Omega::setTimeStep(const Real t){	if(scene) scene->dt=t;}
Real Omega::getTimeStep(){	if(scene) return scene->dt; else return -1; }
void Omega::skipTimeStepper(bool s){ if(scene) scene->setTimeSteppersActive(!s);}

bool Omega::timeStepperActive(){
	if(!scene) return false;
	FOREACH(const shared_ptr<Engine>& e, scene->engines){
		if (isInheritingFrom(e->getClassName(),"TimeStepper")){
			return static_pointer_cast<TimeStepper>(e)->active;
		}
	}
	return false;
}

bool Omega::containTimeStepper(){
	if(!scene) return false;
	FOREACH(const shared_ptr<Engine>& e, scene->engines){
		if (e && isInheritingFrom(e->getClassName(),"TimeStepper")) return true;
	}
	return false;
}



