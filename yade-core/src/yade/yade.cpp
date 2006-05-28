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
#include <yade/yade-lib-factory/ClassFactory.hpp>
#include "Omega.hpp"
#include "FrontEnd.hpp"
#include "Preferences.hpp"

using namespace std;

void firstRunSetup(shared_ptr<Preferences>& pref)
{
	cout <<
"\n\n\
##########################################\n\
#      Yade first run configuration      #\n\
##########################################\n\
\n";
	string 	 guiName
		,guiDir
		,defaultGUI = "QtGUI"
		,defaultDir = "/usr/local/lib/yade/yade-guis";
	char 	 str[500];

	filesystem::path guiFullPath;
	do 
	{
		cout << "1. Specify default GUI name [" << defaultGUI << "] : " ;
		cin.getline(str,500);
		guiName = str;
		if(guiName=="")
			guiName = defaultGUI;
	
		cout << "\n   using GUI: " << guiName << "\n";
	
		cout << "\n2. Specify the path to the default GUI [" << defaultDir << "] : " ;
		cin.getline(str,500);
		guiDir = str;
		
		cout << "\n   using path: " << guiDir << "\n";
		
		if(guiDir=="")
			guiDir = defaultDir;
			
		cout << "\n";
		if (guiDir[guiDir.size()-1]!='/')
			guiDir.push_back('/');
		guiFullPath = filesystem::path(guiDir + ClassFactory::instance().libNameToSystemName(guiName) , filesystem::native);
		if ( ! filesystem::exists(guiFullPath) && guiName != "NullGUI" )
			cout << "## Error: " << guiDir+ClassFactory::instance().libNameToSystemName(guiName) << " doesn't exist\n## Try Again\n\n";
	} while (!filesystem::exists(guiFullPath) && guiName != "NullGUI");
	
	pref->dynlibDirectories.push_back(guiDir.substr(0,guiDir.size()-1));
	pref->defaultGUILibName = guiName;

	IOFormatManager::saveToFile("XMLFormatManager",Omega::instance().yadeConfigPath+"/preferences.xml","preferences",pref);
}

void printHelp()
{
	string flags("");
#ifdef DOUBLE_PRECISION
	flags+="DOUBLE_PRECISION ";
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
\n\
Only one option can be passed to yade, all other options are passed to the selected GUI\n\
";
	if(flags!="")
		cout << "compilation flags: "+ flags +"\n\n";
}

int main(int argc, char *argv[])
{
	Omega::instance().yadeVersionName = "Yet Another Dynamic Engine 0.9.1, beta.";
	
	int ch;
	string 	gui 		= "";
	string 	configPath 	= string(getenv("HOME")) + string("/.yade");
	
	bool 	setup 		= false;
	if( ( ch = getopt(argc,argv,"hnN:wC:c") ) != -1)
		switch(ch)
		{
			case 'h' :	printHelp();		return 1;
			case 'n' :	gui = "NullGUI";	break;
			case 'N' :	gui = optarg; 		break;
			case 'w' :	setup = true;		break;
			case 'C' :	configPath = optarg; 	break;
			case 'c' :	configPath = "."; 	break;
			default	 :	printHelp();		return 1;
		}
	
	if(configPath[configPath.size()-1] == '/')
		configPath = configPath.substr(0,configPath.size()-1); 
	
	Omega::instance().preferences    = shared_ptr<Preferences>(new Preferences);
	Omega::instance().yadeConfigPath = configPath; 
	filesystem::path yadeConfigPath  = filesystem::path(Omega::instance().yadeConfigPath, filesystem::native);
	filesystem::path yadeConfigFile  = filesystem::path(Omega::instance().yadeConfigPath + "/preferences.xml", filesystem::native);

	if ( !filesystem::exists( yadeConfigPath ) || setup || !filesystem::exists(yadeConfigFile) )
	{
		filesystem::create_directories(yadeConfigPath);
		firstRunSetup(Omega::instance().preferences);
	}
	cout << "loading configuration file: " << yadeConfigFile.string() << "\n";
	IOFormatManager::loadFromFile("XMLFormatManager",yadeConfigFile.string(),"preferences",Omega::instance().preferences);

	cout << "Please wait while loading plugins.\n";
	Omega::instance().scanPlugins();
	cout << "Plugins loaded.\n";
	Omega::instance().init();
	
	if( gui.size()==0)
		gui = Omega::instance().preferences->defaultGUILibName;
		
	shared_ptr<FrontEnd> frontEnd = dynamic_pointer_cast<FrontEnd>(ClassFactory::instance().createShared(gui));

 	int ok = frontEnd->run(argc,argv);
	cerr << "Yade: normal exit." << endl;
	return ok;
}

