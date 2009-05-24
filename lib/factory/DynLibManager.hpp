/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Bronek Kozicki                                  *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

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


class DynLibManager 
{
	private :
		#ifdef WIN32
		std::map<const std::string, HINSTANCE> handles;	
		#else	
		std::map<const std::string, void *> handles;
		#endif
		bool autoUnload;

	public :
		DynLibManager ();
		~DynLibManager ();
		void addBaseDirectory(const std::string& dir);

		bool load(const std::string& libName, const std::string& libName2);
		bool load(const std::string& pluginName){return load(libNameToSystemName(pluginName),pluginName);}


		bool unload (const string libName);
		bool isLoaded (const string libName);
		bool unloadAll ();
		void setAutoUnload ( bool enabled );

		string libNameToSystemName(const string& name);
		string systemNameToLibName(const string& name);
		string lastError();
		DECLARE_LOGGER;

	private :
		bool closeLib(const string libName);
		bool error();
		string lastError_;
};


