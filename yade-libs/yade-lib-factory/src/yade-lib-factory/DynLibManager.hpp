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

#include<yade/yade-core/logging.hpp>

using namespace std;

class DynLibManager 
{
	private :
		#ifdef WIN32
		std::map<const string, HINSTANCE> handles;	
		#else	
		std::map<const string, void *> handles;
		#endif

		vector<string> baseDirs;
		bool autoUnload;

	public :
		DynLibManager ();
		~DynLibManager ();
		void addBaseDirectory(const string& dir);

		bool load (const string& libName, const string& libName);
		bool loadFromDirectoryList (const string& fullLibName);

		bool unload (const string libName);
		bool isLoaded (const string libName);
		bool unloadAll ();
		void setAutoUnload ( bool enabled );

		string libNameToSystemName(const string& name);
		string systemNameToLibName(const string& name);
		string findLibDir(const string& name);
		string lastError();
		DECLARE_LOGGER;

	private :
		bool closeLib(const string libName);
		bool error();
		string lastError_;
};

#endif // __DYNLIBMANAGER_H__

