/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifdef EMBED_PYTHON
	#include<Python.h>
	extern int Py_OptimizeFlag;
#endif

#include<signal.h>
#include<cstdlib>
#include <iostream>
#include <string>
#include <getopt.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/preprocessor/stringize.hpp>
#include<yade/lib-factory/ClassFactory.hpp>
#include<yade/lib-base/Logging.hpp>
#include "Omega.hpp"
#include "FrontEnd.hpp"
#include "Preferences.hpp"

using namespace std;

#ifdef LOG4CXX
	// provides parent logger for everybody
	log4cxx::LoggerPtr logger=log4cxx::Logger::getLogger("yade");
#endif

void
sigHandler(int sig){
	#ifdef EMBED_PYTHON
		if(sig==SIGINT){
			LOG_DEBUG("Finalizing Python...");
			Py_Finalize();
			// http://www.cons.org/cracauer/sigint.html
			signal(SIGINT,SIG_DFL); // reset to default
			kill(getpid(),SIGINT); // kill ourselves, this time without Python
		}
	#endif
	#ifdef YADE_DEBUG
		if(sig==SIGABRT || sig==SIGSEGV){
			signal(SIGSEGV,SIG_DFL); signal(SIGABRT,SIG_DFL); // prevent loops - default handlers
			cerr<<"SIGSEGV/SIGABRT handler called; gdb batch file is `"<<Omega::instance().gdbCrashBatch<<"'"<<endl;
			system((string("gdb -x ")+Omega::instance().gdbCrashBatch).c_str());
			unlink(Omega::instance().gdbCrashBatch.c_str()); // delete the crash batch file
			kill(getpid(),sig); // reemit signal after exiting gdb
		}
	#endif
}

void firstRunSetup(shared_ptr<Preferences>& pref)
{
	const char* libDirs[]={"extra","gui","lib","pkg-common","pkg-dem","pkg-fem","pkg-lattice","pkg-mass-spring","pkg-realtime-rigidbody",NULL /* sentinel */};
	string cfgFile=Omega::instance().yadeConfigPath+"/preferences.xml";
	LOG_INFO("Creating default configuration file: "<<cfgFile<<". Please tune by hand if needed.");
	string expLibDir;
	for(int i=0; libDirs[i]!=NULL; i++) {
		expLibDir=string(PREFIX "/lib/yade" SUFFIX "/") + libDirs[i];
		// FIXME: only add if really exists (module may not be built)
		LOG_INFO("Adding plugin directory "<<expLibDir<<".");
		pref->dynlibDirectories.push_back(expLibDir);
	}
	LOG_INFO("Setting GUI: QtGUI.");
	pref->defaultGUILibName="QtGUI";
	IOFormatManager::saveToFile("XMLFormatManager",Omega::instance().yadeConfigPath+"/preferences.xml","preferences",pref);
}

void printHelp()
{
	string flags("");
	flags=flags+"   PREFIX=" PREFIX  "\n";
	flags=flags+"   SUFFIX=" SUFFIX "\n";
	#ifdef YADE_DEBUG
		flags+="   DEBUG";
	#endif
	cerr << 
"\n" << Omega::instance().yadeVersionName << "\n\
\n\
	-h      : print this help.\n\
	-n      : use NullGUI (command line interface) instead of default GUI.\n\
	-N name : use some other custom GUI (none available yet ;)\n\
	-w      : launch the 'first run configuration'\n\
	-c      : use local directory ./ as configuration directory\n\
	-C path : configuration directory different than default ~/.yade/\n\
	-S file : load simulation from file (works with QtGUI only)\n\
	-v      : be verbose (may be repeated)\n\
\n\
	--      : pass all remaining options to the selected GUI\n\
";
	if(flags!="")
		cout << "compilation flags:\n"+ flags +"\n\n";
}


int main(int argc, char *argv[])
{

	// This makes boost stop bitching about dot-files and other files that may not exist on MS-DOS 3.3;
	// see http://www.boost.org/libs/filesystem/doc/portability_guide.htm#recommendations for what all they consider bad.
	// Since it is a static variable, it infulences all boost::filesystem operations in this respect (fortunately).
	filesystem::path::default_name_check(filesystem::native);

	string configPath=string(getenv("HOME")) + "/.yade" SUFFIX;

	#ifdef LOG4CXX
		// read logging configuration from file and watch it (creates a separate thread)a
		std::string logConf=configPath+"/logging.conf";
		if(filesystem::exists(logConf)){
			log4cxx::PropertyConfigurator::configureAndWatch(logConf);
			LOG_INFO("Logger loaded and watches configuration file: "<<logConf<<".");
		} else { // otherwise use simple console-directed logging
			log4cxx::BasicConfigurator::configure();
			logger->setLevel(log4cxx::Level::WARN);
			LOG_INFO("Logger uses basic (console) configuration since `"<<logConf<<"' was not found. INFO and DEBUG messages will be ommited.");
			LOG_INFO("Look at the file doc/logging.conf.sample in the source distribution as an example on how to customize logging.");
		}
	#endif
	
	int ch;
	string gui="";
	string simulationFileName="";
	bool setup=false;
	int verbose=0;
	bool coreOptions=true;
	while(coreOptions && (ch=getopt(argc,argv,"hnN:wC:cvS:"))!=-1)
		switch(ch){
			case 'h': printHelp(); return 1;
			case 'n': gui="NullGUI"; break;
			case 'N': gui=optarg; break;
			case 'w': setup=true; break;
			case 'C': configPath=optarg; break;
			case 'c': configPath="."; break;
			case 'v': verbose+=1; break;
			case 'S': simulationFileName=optarg; break;
			case '-': coreOptions=false; break;
			default: printHelp(); return 1;
		}
	// kill processed options, keep one more which will is in faact non-option (normally the binary)
	argv=&(argv[optind-1]); argc-=optind-1;
	// reset getopt globals for next processing
	optind=0; opterr=0;
	
	if(configPath[configPath.size()-1] == '/')
		configPath = configPath.substr(0,configPath.size()-1); 

	Omega::instance().yadeVersionName = "Yet Another Dynamic Engine 0.11.x, beta, SVN snapshot.";

	Omega::instance().preferences    = shared_ptr<Preferences>(new Preferences);
	Omega::instance().yadeConfigPath = configPath; 
	filesystem::path yadeConfigPath  = filesystem::path(Omega::instance().yadeConfigPath, filesystem::native);
	filesystem::path yadeConfigFile  = filesystem::path(Omega::instance().yadeConfigPath + "/preferences.xml", filesystem::native);

	#ifdef LOG4CXX
		if(verbose==1) logger->setLevel(log4cxx::Level::INFO);
		else if (verbose>=2) logger->setLevel(log4cxx::Level::DEBUG);
	#endif


	#ifdef EMBED_PYTHON
		/* see http://www.python.org/dev/peps/pep-0311 for threading with Python embedded */
		LOG_DEBUG("Initializing Python...");
		Py_OptimizeFlag=1;
		Py_Initialize();
		PyEval_InitThreads(); // this locks the GIL as side-effect
		//PyGILState_STATE pyState=PyGILState_Ensure(); PyGILState_Release(pyState);
		signal(SIGINT,sigHandler);
	#endif

	if (!filesystem::exists(yadeConfigPath) || setup || !filesystem::exists(yadeConfigFile)){
		filesystem::create_directories(yadeConfigPath);
		firstRunSetup(Omega::instance().preferences);
	}

	#ifdef YADE_DEBUG
		// postponed until the config dir has been created
		ofstream gdbBatch;
		Omega::instance().gdbCrashBatch=(yadeConfigPath/"gdb_crash_batch-pid").string()+lexical_cast<string>(getpid());
		gdbBatch.open(Omega::instance().gdbCrashBatch.c_str()); gdbBatch<<"attach "<<lexical_cast<string>(getpid())<<"\nthread info\nthread apply all backtrace\n"; gdbBatch.close();
		signal(SIGABRT,sigHandler);
		signal(SIGSEGV,sigHandler);
		LOG_DEBUG("ABRT/SEGV signal handlers set, crash batch created as "<<Omega::instance().gdbCrashBatch);
	#endif

	LOG_INFO("Loading configuration file: "<<yadeConfigFile.string());
	IOFormatManager::loadFromFile("XMLFormatManager",yadeConfigFile.string(),"preferences",Omega::instance().preferences);

	LOG_INFO("Loading plugins...");
	Omega::instance().scanPlugins();
	LOG_INFO("Plugins loaded.");
	Omega::instance().init();

	Omega::instance().setSimulationFileName(simulationFileName); //init() resets to "";
	
	if(gui.size()==0) gui=Omega::instance().preferences->defaultGUILibName;
		
	shared_ptr<FrontEnd> frontEnd = dynamic_pointer_cast<FrontEnd>(ClassFactory::instance().createShared(gui));
	
 	// for(int i=0;i<argc; i++)cerr<<"Argument "<<i<<": "<<argv[i]<<endl;
	int ok = frontEnd->run(argc,argv);

	#ifdef EMBED_PYTHON
		// LOG_DEBUG("Finalizing Python...");
		// Py_Finalize(); // FIXME: http://www.boost.org/libs/python/todo.html#pyfinalize-safety says this is unsafe with boost::python
	#endif
	#ifdef YADE_DEBUG
		unlink(Omega::instance().gdbCrashBatch.c_str());
	#endif

	LOG_INFO("Yade: normal exit.");
	return ok;
}

