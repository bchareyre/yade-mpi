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
#include<yade/lib/base/Math.hpp>
#include<yade/lib/multimethods/FunctorWrapper.hpp>
#include<yade/lib/multimethods/Indexable.hpp>
#include<cstdlib>
#include<boost/filesystem/operations.hpp>
#include<boost/filesystem/convenience.hpp>
#include<boost/filesystem/exception.hpp>
#include<boost/algorithm/string.hpp>
#include<boost/thread/mutex.hpp>
#include<boost/version.hpp>
#include<boost/python.hpp>

#include<yade/lib/serialization/ObjectIO.hpp>


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

const map<string,DynlibDescriptor>& Omega::getDynlibsDescriptor(){return dynlibs;}

const shared_ptr<Scene>& Omega::getScene(){return scene;}
void Omega::resetScene(){ RenderMutexLock lock; scene = shared_ptr<Scene>(new Scene);}

Real Omega::getRealTime(){ return (microsec_clock::local_time()-startupLocalTime).total_milliseconds()/1e3; }
time_duration Omega::getRealTime_duration(){return microsec_clock::local_time()-startupLocalTime;}


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
	stop();
	init();
}

void Omega::init(){
	sceneFile="";
	resetScene();
	sceneAnother=shared_ptr<Scene>(new Scene);
	timeInit();
	createSimulationLoop();
}

void Omega::timeInit(){
	startupLocalTime=microsec_clock::local_time();
}

void Omega::createSimulationLoop(){	simulationLoop=shared_ptr<ThreadRunner>(new ThreadRunner(&simulationFlow_));}
void Omega::stop(){ LOG_DEBUG("");  if (simulationLoop&&simulationLoop->looping())simulationLoop->stop(); if (simulationLoop) simulationLoop=shared_ptr<ThreadRunner>(); }

/* WARNING: even a single simulation step is run asynchronously; the call will return before the iteration is finished. */
void Omega::step(){
	if (simulationLoop){
		simulationLoop->spawnSingleAction();
	}
}

void Omega::run(){
	if(!simulationLoop){ LOG_ERROR("No Omega::simulationLoop? Creating one (please report bug)."); createSimulationLoop(); }
	if (simulationLoop && !simulationLoop->looping()){
		simulationLoop->start();
	}
}


void Omega::pause(){
	if (simulationLoop && simulationLoop->looping()){
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
		catch (std::runtime_error& e){
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
				if(getenv("YADE_DEBUG")){ cerr<<"["<<*I<<"]"; PyErr_Print(); }
				boost::python::handle_exception();
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

void Omega::loadSimulation(const string& f, bool quiet){
	bool isMem=algorithm::starts_with(f,":memory:");
	if(!isMem && !filesystem::exists(f)) throw runtime_error("Simulation file to load doesn't exist: "+f);
	if(isMem && memSavedSimulations.count(f)==0) throw runtime_error("Cannot load nonexistent memory-saved simulation "+f);
	
	if(!quiet) LOG_INFO("Loading file "+f);
	{
		stop(); // stop current simulation if running
		resetScene();
		RenderMutexLock lock;
		if(isMem){
			istringstream iss(memSavedSimulations[f]);
			yade::ObjectIO::load<typeof(scene),boost::archive::binary_iarchive>(iss,"scene",scene);
		} else {
			yade::ObjectIO::load(f,"scene",scene);
		}
	}
	if(scene->getClassName()!="Scene") throw logic_error("Wrong file format (scene is not a Scene!?) in "+f);
	sceneFile=f;
	timeInit();
	if(!quiet) LOG_DEBUG("Simulation loaded");
}



void Omega::saveSimulation(const string& f, bool quiet){
	if(f.size()==0) throw runtime_error("f of file to save has zero length.");
	if(!quiet) LOG_INFO("Saving file " << f);
	if(algorithm::starts_with(f,":memory:")){
		if(memSavedSimulations.count(f)>0 && !quiet) LOG_INFO("Overwriting in-memory saved simulation "<<f);
		ostringstream oss;
		yade::ObjectIO::save<typeof(scene),boost::archive::binary_oarchive>(oss,"scene",scene);
		memSavedSimulations[f]=oss.str();
	}
	else {
		// handles automatically the XML/binary distinction as well as gz/bz2 compression
		yade::ObjectIO::save(f,"scene",scene); 
	}
	sceneFile=f;
}




