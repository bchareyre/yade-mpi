/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Omega.hpp"
#include "yadeExceptions.hpp"
#include "MetaBody.hpp"
#include "ThreadRunner.hpp"
#include "Preferences.hpp"
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/lib-serialization/IOFormatManager.hpp>
#include<yade/lib-serialization/IOFormatManager.hpp>
#include<yade/lib-multimethods/FunctorWrapper.hpp>
#include<yade/lib-multimethods/Indexable.hpp>
#include <cstdlib>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include <cxxabi.h>

CREATE_LOGGER(Omega);

Omega::Omega()
{
	LOG_INFO("Constructing Omega  (if multiple times - check '-rdynamic' flag!).");
}


Omega::~Omega()
{
	*logFile << "\t" << "<Summary Duration=\"" << sStartingSimulationTime-second_clock::local_time() << "\">" <<endl;
	*logFile << "</Simulation>" << endl << endl;
	logFile->close();
}


void Omega::init()
{
	simulationFileName="";
	currentIteration = 0;
	dt = 0.01;
	logFile = shared_ptr<ofstream>(new ofstream("../data/log.xml", ofstream::out | ofstream::app));
}


void Omega::logError(const string& str)
{
	*logFile << "\t" << "<Error Date=\"" << sStartingSimulationTime-second_clock::local_time() << "\" " << "Message =\""<< str << "\"" << endl;
}


void Omega::logMessage(const string& str)
{
	*logFile << "\t" << "<Message Date=\"" << sStartingSimulationTime-second_clock::local_time() << "\" " << "Message =\""<< str << "\"" << endl;
}


void Omega::createSimulationLoop()
{
	simulationLoop   = shared_ptr<ThreadRunner>(new ThreadRunner(&simulationFlow_));
}


void Omega::finishSimulationLoop()
{
	if (simulationLoop)
		simulationLoop->stop();
}


void Omega::joinSimulationLoop()
{
	if (simulationLoop)
	{
		simulationLoop->stop();
		simulationLoop   = shared_ptr<ThreadRunner>();
	}
}


void Omega::spawnSingleSimulationLoop()
{
	if (simulationLoop)
	{
		msStartingPauseTime = microsec_clock::local_time();
		simulationLoop->spawnSingleAction();
	}
}


void Omega::startSimulationLoop()
{
	if (simulationLoop && !simulationLoop->looping())
	{
		simulationPauseDuration += microsec_clock::local_time()-msStartingPauseTime;
		simulationLoop->start();
	}
}


void Omega::stopSimulationLoop()
{
	if (simulationLoop && simulationLoop->looping())
	{
		msStartingPauseTime = microsec_clock::local_time();
		simulationLoop->stop();
	}
}


void Omega::buildDynlibDatabase(const vector<string>& dynlibsList)
{	

	vector< string >::const_iterator dlli    = dynlibsList.begin();
	vector< string >::const_iterator dlliEnd = dynlibsList.end();
	for( ; dlli!=dlliEnd ; ++dlli)
	{
		string name = *dlli;
		//cerr<<"Library: "<<name<<"\n";
		shared_ptr<Factorable> f;
		try
		{
			f = ClassFactory::instance().createShared(name);
			dynlibs[name].isIndexable    = dynamic_pointer_cast<Indexable>(f);
			dynlibs[name].isFactorable   = dynamic_pointer_cast<Factorable>(f);
			dynlibs[name].isSerializable = dynamic_pointer_cast<Serializable>(f);
			for(int i=0;i<f->getBaseClassNumber();i++)
				dynlibs[name].baseClasses.insert(f->getBaseClassName(i));
		}
		catch (FactoryError&)
		{
		}
	}

	map<string,DynlibDescriptor>::iterator dli    = dynlibs.begin();
	map<string,DynlibDescriptor>::iterator dliEnd = dynlibs.end();
	for( ; dli!=dliEnd ; ++dli)
	{
		set<string>::iterator bci    = (*dli).second.baseClasses.begin();
		set<string>::iterator bciEnd = (*dli).second.baseClasses.end();
		for( ; bci!=bciEnd ; ++bci)
		{
			string name = *bci;
			if (name=="MetaDispatchingEngine1D" || name=="MetaDispatchingEngine2D")
				(*dli).second.baseClasses.insert("MetaEngine");
			else if (name=="EngineUnit1D" || name=="EngineUnit2D")
				(*dli).second.baseClasses.insert("EngineUnit");
			else if (name=="Serializable")
				(*dli).second.baseClasses.insert("Factorable");
			else if (name!="Factorable" && name!="Indexable")
			{
				shared_ptr<Factorable> f = ClassFactory::instance().createShared(name);
				for(int i=0;i<f->getBaseClassNumber();i++)
					dynlibs[name].baseClasses.insert(f->getBaseClassName(i));
			}
		}
	}

/*
	dli    = dynlibs.begin();
	dliEnd = dynlibs.end();
	for( ; dli!=dliEnd ; ++dli)
	{
		cerr << (*dli).first << " : " ;
		set<string>::iterator bci    = (*dli).second.baseClasses.begin();
		set<string>::iterator bciEnd = (*dli).second.baseClasses.end();
		for( ; bci!=bciEnd ; ++bci)
			cerr << *bci << endl;
		cerr << endl;
	}*/
}


bool Omega::isInheritingFrom(const string& className, const string& baseClassName )
{
	return (dynlibs[className].baseClasses.find(baseClassName)!=dynlibs[className].baseClasses.end());
}


void Omega::scanPlugins()
{
// #define STUPID_DLL
//	ClassFactory::instance().unloadAll();

	// TODO: remove all comments of the form /*DLL ... */, they comment the old code that whould not be needed any more

	vector<string>::iterator dldi    = preferences->dynlibDirectories.begin();
	vector<string>::iterator dldiEnd = preferences->dynlibDirectories.end();
	for( ; dldi != dldiEnd ; ++dldi)
		ClassFactory::instance().addBaseDirectory((*dldi));
			
	vector<string> dynlibsList;
	#ifdef STUPID_DLL
		vector< int >    dynlibsListLoaded;
	#endif

	vector<string>::iterator si = preferences->dynlibDirectories.begin();
	vector<string>::iterator siEnd = preferences->dynlibDirectories.end();
	for( ; si != siEnd ; ++si)
	{
		filesystem::path directory((*si));

		if ( filesystem::exists( directory ) )
		{
			filesystem::directory_iterator di( directory );
			filesystem::directory_iterator diEnd;
			for ( ; di != diEnd; ++di )
			{
				// node is not a directory and is either regular file or non-dangling symlink; and extension is not ".a"; AND moreover transforming it to library name and back to filename is identity; otherwise the file wouldn't be loaded by the DynLibManager anyway
				if (!filesystem::is_directory(*di) && filesystem::exists(*di) && filesystem::extension(*di)!=".a" &&
					ClassFactory::instance().libNameToSystemName(ClassFactory::instance().systemNameToLibName(filesystem::basename(*di)))==(di->leaf())){
					filesystem::path name(filesystem::basename((*di)));
					int prevLength = (*di).leaf().size();
					int length = name.leaf().size();
					// warning: this can produce invalid name (too short).
					// 0-length names are dumped directly
					// names 0<length<4 should fail assertion in DynLibManager::systemNameToLibName
					// the whole loading "logic" should be rewritten from scratch...
					//
					// my reading of what it is supposed to do: returns the part of filename before the first dot... !!??
					// humm, perhaps it is a result of obfuscative c++ contest (obfuscative c++ = oxymoron, anyway)
					while (length!=prevLength)
					{
						prevLength=length;
						name = filesystem::path(filesystem::basename(name));
						length = name.leaf().size();
					}
					if(name.leaf().length()<1) continue; // filter out 0-length filenames
					if(dynlibsList.size()==0 || ClassFactory::instance().systemNameToLibName(name.leaf())!=dynlibsList.back()) {
						LOG_DEBUG("Added plugin: "<<*si<<"/"<<di->leaf()<<".");
						dynlibsList.push_back(ClassFactory::instance().systemNameToLibName(name.leaf()));
						#ifdef STUPID_DLL
							dynlibsListLoaded.push_back(false);
						#endif
					}
					else LOG_DEBUG("Possible plugin discarded: "<<*si<<"/"<<name.leaf()<<".");
				} else LOG_DEBUG("File not considered a plugin: "<<di->leaf()<<".");
			}
		}
		else LOG_ERROR("Nonexistent plugin directory: "<<directory.native_directory_string()<<".");
	}

	bool allLoaded = false;
	vector<string> dynlibsClassList; // dynlibsList holds filenames, this holds classes defined inside (may be different if using yadePuginClasses)
	#ifdef STUPID_DLL
		int overflow = 30; // to prevent infinite loop
		assert(dynlibsList.size() == dynlibsListLoaded.size());
		while (!allLoaded && --overflow > 0){
		int loaded = 0;
	#endif
		vector< string >::iterator dlli    = dynlibsList.begin();
		vector< string >::iterator dlliEnd = dynlibsList.end();
		allLoaded = true;
		for( ; dlli!=dlliEnd ; ++dlli
		#ifdef STUPID_DLL
		, ++loaded
		#endif
		)
		{
			#ifdef STUPID_DLL
			if( dynlibsListLoaded[loaded] == false ) {
				LOG_DEBUG("Trying to load plugin "<<*dlli<<" (no."<<loaded<<"; overflow="<<overflow<<")");
			#endif
			bool thisLoaded = ClassFactory::instance().load((*dlli));
			if (!thisLoaded
				#ifdef STUPID_DLL
				&& overflow == 1
				#endif
				){
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
				else LOG_ERROR("Error loading Library `"<<(*dlli)<<"': "<<err<<" ."); // leave space to not to confuse c++filt
			}
			#ifdef STUPID_DLL
			else if (!thisLoaded) { LOG_DEBUG("Plugin "<<*dlli<<" not loaded successfully this time: "<<ClassFactory::instance().lastError()<<" (will retry)."); }
			#endif
			else { // no error
				if (ClassFactory::instance().lastPluginClasses().size()==0){ // regular plugin, one class per file
					dynlibsClassList.push_back(*dlli);
					LOG_DEBUG("Plugin "<<*dlli<<" loaded succesfully.");
				} else {// if plugin defines yadePluginClasses (has multiple classes), insert these into dynLibsList
					vector<string> css=ClassFactory::instance().lastPluginClasses();
					for(size_t i=0; i<css.size();i++) { dynlibsClassList.push_back(css[i]); LOG_DEBUG("Plugin "<<*dlli<<": added class "<<css[i]<<".");  }
				}
			}
			allLoaded &= thisLoaded;
			#ifdef STUPID_DLL
				if(thisLoaded) dynlibsListLoaded[loaded] = true; }
			#endif
		}
	#ifdef STUPID_DLL
	}
	#endif

	if(!allLoaded) LOG_WARN("Couldn't load everything, some stuff may work incorrectly.");
	
	buildDynlibDatabase(dynlibsClassList);
}


// bool Omega::getDynlibDescriptor(const string& libName, string& type)
// {
// 	//LOCK(omegaMutex);
// 		
// 	map<string,DynlibDescriptor>::iterator it = dynlibs.find(libName);
// 	if (it!=dynlibs.end())
// 	{
// 		type = (*it).second.baseClass;
// 		return true;
// 	}
// 	else
// 		return false;
// }


const map<string,DynlibDescriptor>& Omega::getDynlibsDescriptor()
{
	return dynlibs;
}


void Omega::incrementCurrentIteration()
{
	++currentIteration;
}


long int Omega::getCurrentIteration()
{
	return currentIteration;
}


void Omega::setSimulationFileName(const string f)
{
	simulationFileName = f;
}


string Omega::getSimulationFileName()
{
	return simulationFileName;
}


void Omega::loadSimulation()
{
	if(	    (Omega::instance().getSimulationFileName().size() != 0)
		&&  (filesystem::exists(simulationFileName)) 
		&&  (filesystem::extension(simulationFileName)==".xml" || filesystem::extension(simulationFileName)==".yade" ))
	{
		freeRootBody();
		logMessage("Loading file " + simulationFileName);
		
		if(filesystem::extension(simulationFileName)==".xml")
			IOFormatManager::loadFromFile("XMLFormatManager",simulationFileName,"rootBody",rootBody);
		else if(filesystem::extension(simulationFileName)==".yade" )
			IOFormatManager::loadFromFile("BINFormatManager",simulationFileName,"rootBody",rootBody);
		if( rootBody->getClassName() != "MetaBody")
			throw yadeBadFile("bad file to load");

		sStartingSimulationTime = second_clock::local_time();
		msStartingSimulationTime = microsec_clock::local_time();
		simulationPauseDuration = msStartingSimulationTime-msStartingSimulationTime;
		msStartingPauseTime = msStartingSimulationTime;
		*logFile << "<Simulation" << " Date =\"" << sStartingSimulationTime << "\">" << endl;
		currentIteration = 0;
		simulationTime = 0;	
	}
	else
	{
		std::string error = yadeExceptions::BadFile + simulationFileName;
		cerr << error.c_str() << endl;
		throw yadeBadFile(error.c_str());
	}
}

void Omega::saveSimulation(const string name)
{
	if(	   (name.size() != 0)
		&& (filesystem::extension(name)==".xml" || filesystem::extension(name)==".yade") )
        {
		logMessage("Saving file " + name);
		if(filesystem::extension(name)==".xml")
			IOFormatManager::saveToFile("XMLFormatManager",name,"rootBody",rootBody);
		else if(filesystem::extension(name)==".yade" )
			IOFormatManager::saveToFile("BINFormatManager",name,"rootBody",rootBody);
	}
	else
	{
		std::string error = yadeExceptions::BadFile + name;
		cerr << error.c_str() << endl;
		throw yadeBadFile(error.c_str());
	}
}

void Omega::freeRootBody()
{
	rootBody = shared_ptr<MetaBody>();
}


// FIXME - remove that
void Omega::setTimeStep(const Real t)
{
	dt = t;
}


Real Omega::getTimeStep()
{
	return dt;
}


void Omega::skipTimeStepper(bool s)
{
	rootBody->setTimeSteppersActive(!s);
}

//FIXME : make that recursive to scan all submetabodies ???
bool Omega::containTimeStepper()
{
	vector<shared_ptr<Engine> >::iterator ai    = rootBody->engines.begin();
	vector<shared_ptr<Engine> >::iterator aiEnd = rootBody->engines.end();
	for(int i=0;ai!=aiEnd;++ai,i++)
	{
		if (isInheritingFrom((*ai)->getClassName(),"TimeStepper"))
			return true;
// 
/*		map<string,DynlibDescriptor>::const_iterator dli = Omega::instance().getDynlibsType().find((*ai)->getClassName());
		if (dli!=Omega::instance().getDynlibsType().end())
		{
			if ((*dli).second.baseClass=="TimeStepper")
				return true;
		}*/
	}
        return false;
}

const shared_ptr<MetaBody>& Omega::getRootBody()
{
	return rootBody;
}

void Omega::setRootBody(shared_ptr<MetaBody>& rb){ rootBody=rb;}


ptime Omega::getMsStartingSimulationTime()
{
	return msStartingSimulationTime;
}


time_duration Omega::getSimulationPauseDuration()
{
	return simulationPauseDuration;
}



boost::mutex& Omega::getRootBodyMutex()
{
	return rootBodyMutex;
}


