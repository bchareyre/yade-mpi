/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <getopt.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade-lib-factory/ClassFactory.hpp>
#include <yade/Omega.hpp>
#include <yade/FrontEnd.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Preferences.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void printHelp()
{
	cout << endl;
	cout << "Yet Another Dynamic Engine, beta. " << endl;
	cout << endl;
	cout << "	-h	: print this help" << endl;
	cout << "	-n	: use NullGUI instead of default one" << endl;
	cout << "	-g	: change default GUI" << endl;
	cout << "	-d	: add plugin directory" << endl;
	cout << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void changeDefaultGUI(shared_ptr<Preferences>& pref)
{
	string guiName;
	string guiDir;
	filesystem::path guiFullPath;

	do 
	{
		cout << endl;
		cout << endl;
		cout << "1. Specify default GUI name [QtGUI/NullGUI/...] : " ;
		cin >> guiName;
		cout << endl;
		cout << "2. Specify the path where to find it : " ;
		cin >> guiDir;
		cout << endl;

		if (guiDir[guiDir.size()-1]!='/')
			guiDir.push_back('/');

		guiFullPath = filesystem::path(guiDir+ClassFactory::instance().libNameToSystemName(guiName), filesystem::native);

		if (!filesystem::exists(guiFullPath))
		{
			cout << "##"<<endl;
			cout << "## Error : could not find file " << guiDir+ClassFactory::instance().libNameToSystemName(guiName) << endl;
			cout << "## Try Again" << endl;
			cout << "##"<<endl;
			cout << endl;
		}

	} while (!filesystem::exists(guiFullPath));
	
	pref->dynlibDirectories.push_back(guiDir.substr(0,guiDir.size()-1));
	pref->defaultGUILibName = guiName;

	string yadeConfigPath = string(getenv("HOME")) + string("/.yade");
	IOManager::saveToFile("XMLManager",yadeConfigPath+"/preferences.xml","preferences",pref);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void addPluginDirectory(shared_ptr<Preferences>& pref)
{
	string directory;
	filesystem::path path;
	do 
	{
		cout << endl;
		cout << endl;
		cout << "1. New plugin directory : " ;
		cin >> directory;
		cout << endl;

		if (directory[directory.size()-1]!='/')
			directory.push_back('/');

		path = filesystem::path(directory, filesystem::native);

		if (!filesystem::exists(path))
		{
			cout << "##"<<endl;
			cout << "## Error : could not find directory " << directory << endl;
			cout << "## Try Again" << endl;
			cout << "##"<<endl;
			cout << endl;
		}

	} while (!filesystem::exists(path));
	
	pref->dynlibDirectories.push_back(directory.substr(0,directory.size()-1));

	string yadeConfigPath = string(getenv("HOME")) + string("/.yade");
	IOManager::saveToFile("XMLManager",yadeConfigPath+"/preferences.xml","preferences",pref);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
///
/// First time yade is loaded
///
	Omega::instance().preferences = shared_ptr<Preferences>(new Preferences);

	filesystem::path yadeConfigPath = filesystem::path(string(getenv("HOME")) + string("/.yade"), filesystem::native);
	if ( !filesystem::exists( yadeConfigPath ) )
	{
		filesystem::create_directories(yadeConfigPath);

		cout << endl;
		cout << endl;
		cout << "################################################" << endl;
		cout << "#	Welcome to Yade first use wizard	#" << endl;
		cout << "################################################" << endl;

		changeDefaultGUI(Omega::instance().preferences);
	}

	IOManager::loadFromFile("XMLManager",yadeConfigPath.string()+"/preferences.xml","preferences",Omega::instance().preferences);

///
/// Checks for command line argument
///
	int ch;
	bool useNullGUI = false;
	bool useNonDefaultGUI = false;
	if( ( ch = getopt(argc,argv,"hngd") ) != -1)
		switch(ch)
		{
			case 'h' :	printHelp();
					return 1;
			case 'n' :	useNullGUI = true;
					break;
			case 'g' :	changeDefaultGUI(Omega::instance().preferences);
					break;
			case 'd' :	addPluginDirectory(Omega::instance().preferences);
					break;
			default	 :	printHelp();
					return 1;
		}
	
	shared_ptr<FrontEnd> frontEnd;

	cout << "Please wait while loading plugins.........................." << endl;
	Omega::instance().scanPlugins();
	cout << "Plugins loaded."<<endl;

	Omega::instance().init();

	if (useNullGUI)
		frontEnd = dynamic_pointer_cast<FrontEnd>(ClassFactory::instance().createShared("NullGUI"));
	else
		frontEnd = dynamic_pointer_cast<FrontEnd>(ClassFactory::instance().createShared(Omega::instance().preferences->defaultGUILibName));

 	int ok = frontEnd->run(argc,argv);
	
	cout << "Yade: normal exit." << endl;

	return ok;
}
