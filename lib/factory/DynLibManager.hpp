/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Bronek Kozicki                                  *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <dlfcn.h>

#include <string>
#include <iostream>
#include <map>
#include <vector>

#include<yade/lib-base/Logging.hpp>

using namespace std;


class DynLibManager 
{
	private :
		std::map<const std::string, void *> handles;
		bool autoUnload;

	public :
		DynLibManager ();
		~DynLibManager ();
		void addBaseDirectory(const std::string& dir);

		bool load(const std::string& libName);

		bool unload (const string& libName);
		bool isLoaded (const string& libName);
		bool unloadAll ();
		void setAutoUnload ( bool enabled );

		string lastError();
		DECLARE_LOGGER;

	private :
		bool closeLib(const string libName);
		bool error();
		string lastError_;
};


