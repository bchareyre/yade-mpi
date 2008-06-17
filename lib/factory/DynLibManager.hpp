/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Bronek Kozicki                                  *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef DYNLIBMANAGER_HPP
#define DYNLIBMANAGER_HPP

#ifdef WIN32
	#define OS "Windows"
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h> 
#else
	#include <dlfcn.h>
	#define OS "Linux"
#endif

#include <string>
#include <iostream>
#include <map>
#include <vector>

#include<yade/lib-base/Logging.hpp>

using namespace std;

/*! Macro defining what classes can be found in this plugin -- must always be used in the respective .cpp file.

If left empty, filename will be used to deduce that.
*/
#define YADE_PLUGIN(...) const char* yadePluginClasses[]={ __VA_ARGS__ "", NULL };

class DynLibManager 
{
	private :
		#ifdef WIN32
		std::map<const std::string, HINSTANCE> handles;	
		#else	
		std::map<const std::string, void *> handles;
		#endif

		vector<string> baseDirs;
		bool autoUnload;

	public :
		DynLibManager ();
		~DynLibManager ();
		void addBaseDirectory(const std::string& dir);

		bool load (const std::string& libName, const std::string& libName2);
		bool loadFromDirectoryList (const std::string& fullLibName);

		bool unload (const string libName);
		bool isLoaded (const string libName);
		bool unloadAll ();
		void setAutoUnload ( bool enabled );

		string libNameToSystemName(const string& name);
		string systemNameToLibName(const string& name);
		string findLibDir(const string& name);
		string lastError();
		vector<string> lastPluginClasses;
		DECLARE_LOGGER;

	private :
		bool closeLib(const string libName);
		bool error();
		string lastError_;
};

#endif // __DYNLIBMANAGER_H__

