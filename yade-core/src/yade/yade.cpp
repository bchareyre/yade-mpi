/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include <iostream>
#include <string>
#include <getopt.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <yade/yade-lib-factory/ClassFactory.hpp>
#include <yade/yade-lib-base/Logging.hpp>
#include "Omega.hpp"
#include "FrontEnd.hpp"
#include "Preferences.hpp"


using namespace std;

#ifdef LOG4CXX
// provides parent logger for everybody
log4cxx::LoggerPtr logger=log4cxx::Logger::getLogger("yade");
#endif

// FIXME - those two function will be moved to some class responsible for configuration
std::string getPrefix()
{
	// I had to use BOOST_PP_STRINGIZE because g++3.3 tried to insert the path verbatim (not as a string)
	// and there was a compilation error
	std::string tmp(BOOST_PP_STRINGIZE(PREFIX));
	if(tmp=="PREFIX") // if PREFIX is undefined, BOOST_PP_STRINGIZE returns the name instead of empty value
	{
		LOG_FATAL("undefined PREFIX, aborting");
		exit(1);
	};
	return tmp;
};

std::string getPostfix()
{
	std::string tmp(BOOST_PP_STRINGIZE(POSTFIX));
	if(tmp=="POSTFIX") tmp=""; // if POSTFIX is undefined, BOOST_PP_STRINGIZE returns the name instead of empty value
	return tmp;
};

void firstRunSetup(shared_ptr<Preferences>& pref)
{
	char *libDirs[]={"yade-extra","yade-guis","yade-libs","yade-package-common","yade-package-dem","yade-package-fem","yade-package-lattice","yade-package-mass-spring","yade-package-realtime-rigidbody",NULL /* sentinel */};
	string cfgFile=Omega::instance().yadeConfigPath+"/preferences.xml";
	LOG_INFO("Creating default configuration file: "<<cfgFile<<". Please tune by hand if needed.");
	string expLibDir;
	for(int i=0; libDirs[i+1]!=NULL; i++) {
		expLibDir=getPrefix() + "/lib/yade" + getPostfix() + "/" + libDirs[i];
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
	flags=flags+"PREFIX=" +getPrefix()+ "\n";
	flags=flags+"POSTFIX=" + getPostfix() + "\n";
#ifdef SINGLE_PRECISION
	flags+="DOUBLE_PRECISION ";
#endif
#ifdef DOUBLE_PRECISION
	flags+="DOUBLE_PRECISION ";
#endif
#ifdef QGLVIEWER20
	flags+="QGLVIEWER20 ";
#endif
#ifdef NO_GLUTINIT
	flags+="NO_GLUTINIT ";
#endif
#ifdef DEBUG
	flags+="DEBUG (is this flag used?) ";
#endif
	cout << 
"\n" << Omega::instance().yadeVersionName << "\n\
\n\
	-h	: print this help.\n\
	-n	: use NullGUI (command line interface) instead of default GUI.\n\
	-N name : use some other custom GUI (none available yet ;)\n\
	-w	: launch the 'first run configuration'\n\
	-c	: use local directory ./ as configuration directory\n\
	-C path : configuration directory different than default ~/.yade/\n\
	-S file : load simulation from file (works with QtGUI only)\n\
\n\
Only one option can be passed to yade, all other options are passed to the selected GUI\n\
";
	if(flags!="")
		cout << "compilation flags:\n"+ flags +"\n\n";
}


int main(int argc, char *argv[])
{

	int ch;
	string gui = "";
	string configPath = string(getenv("HOME")) + "/.yade" + getPostfix();
	string simulationFileName="";

	// This makes boost stop bitching about dot-files and other files that may not exist on MS-DOS 3.3;
	// see http://www.boost.org/libs/filesystem/doc/portability_guide.htm#recommendations for what all they consider bad.
	// Since it is a static variable, it infulences all boost::filesystem operations in this respect (fortunately).
	filesystem::path::default_name_check(filesystem::native);

#	ifdef LOG4CXX
	// read logging configuration from file and watch it (creates a separate thread)a
	std::string logConf=configPath+"/logging.conf";
	if(filesystem::exists(logConf)){
		log4cxx::PropertyConfigurator::configureAndWatch(logConf);
		LOG_INFO("Logger loaded and watches configuration file: "<<logConf<<".");
	} else { // otherwise use simple console-directed logging
		log4cxx::BasicConfigurator::configure();
		LOG_INFO("Logger uses basic (console) configuration ("<<logConf<<" not found). Look at the file yade-doc/logging.conf.sample in the source distribution on an example how to customize logging.");
	}
#	endif

	
	bool 	setup 		= false;
	if( ( ch = getopt(argc,argv,"hnN:wC:cS:") ) != -1)
		switch(ch)
		{
			case 'h' :	printHelp();		return 1;
			case 'n' :	gui = "NullGUI";	break;
			case 'N' :	gui = optarg; 		break;
			case 'w' :	setup = true;		break;
			case 'C' :	configPath = optarg; 	break;
			case 'c' :	configPath = "."; 	break;
			case 'S' : simulationFileName=optarg; break;
			default	 :	printHelp();		return 1;
		}
	
	if(configPath[configPath.size()-1] == '/')
		configPath = configPath.substr(0,configPath.size()-1); 

	Omega::instance().yadeVersionName = "Yet Another Dynamic Engine 0.10.0, beta.";

	Omega::instance().preferences    = shared_ptr<Preferences>(new Preferences);
	Omega::instance().yadeConfigPath = configPath; 
	filesystem::path yadeConfigPath  = filesystem::path(Omega::instance().yadeConfigPath, filesystem::native);
	filesystem::path yadeConfigFile  = filesystem::path(Omega::instance().yadeConfigPath + "/preferences.xml", filesystem::native);

	if ( !filesystem::exists( yadeConfigPath ) || setup || !filesystem::exists(yadeConfigFile) )
	{
		filesystem::create_directories(yadeConfigPath);
		firstRunSetup(Omega::instance().preferences);
	}
	//cout << "loading configuration file: " << yadeConfigFile.string() << "\n";
	LOG_INFO("Loading configuration file: "<<yadeConfigFile.string());

	IOFormatManager::loadFromFile("XMLFormatManager",yadeConfigFile.string(),"preferences",Omega::instance().preferences);

	LOG_INFO("Please wait while loading plugins.");
	Omega::instance().scanPlugins();
	LOG_INFO("Plugins loaded.");
	Omega::instance().init();

	Omega::instance().setSimulationFileName(simulationFileName); //init() resets to "";
	
	if( gui.size()==0)
		gui = Omega::instance().preferences->defaultGUILibName;
		
	shared_ptr<FrontEnd> frontEnd = dynamic_pointer_cast<FrontEnd>(ClassFactory::instance().createShared(gui));

 	int ok = frontEnd->run(argc,argv);
	LOG_INFO("Yade: normal exit.");
	return ok;
}

